#include "search.h"
#include "board.h"
#include <algorithm>
#include <iostream>
#include <functional>

int SearchEngine::pieceValue(Pieces p) {
    switch (p) {
      case PAWN_W:   case PAWN_B:   return 100;
      case KNIGHT_W: case KNIGHT_B: return 320;
      case BISHOP_W: case BISHOP_B: return 330;
      case ROOK_W:   case ROOK_B:   return 500;
      case QUEEN_W:  case QUEEN_B:  return 900;
      case KING_W:   case KING_B:   return 20000;
    }
    return 0;
}

SearchEngine::SearchEngine(EvaluationEngine& engine)
  : evalEngine(engine), timeLimitMs(1000), searchDepth(4)
{
    for(int i=0;i<MAX_PLY;i++) {
      killers[i][0] = killers[i][1] = Move{};
      for(int j=0;j<64;j++) historyScores[i][j] = 0;
    }
}

void SearchEngine::setTimeLimit(int ms) {
    timeLimitMs = ms;
}

Move SearchEngine::addMove(const Board&, int from, int to, Pieces pieceType) {
    Move m{};
    m.fromSquare = from;
    m.toSquare   = to;
    m.pieceType  = pieceType;
    m.promotion  = pieceType;
    m.castle     = 0;
    m.validation = 0;
    m.score      = 0;
    m.exhausted  = false;
    return m;
}

std::vector<Move> SearchEngine::generateLegalMoves(Board& board) {
    std::vector<Move> moves;
    BitBoard ownOcc = board.turn==1 ? board.board_occupancy_white : board.board_occupancy_black;
    BitBoard enmOcc = board.turn==1 ? board.board_occupancy_black : board.board_occupancy_white;
    bool whiteToPlay = board.turn==1;
    auto pushOrCap = [&](int from, int to){
      if (whiteToPlay ? (to/8)==0 : (to/8)==7) {
        if (whiteToPlay) {
          for (auto promo : {QUEEN_W, ROOK_W, BISHOP_W, KNIGHT_W}) {
            Move m = addMove(board,from,to,PAWN_W);
            m.promotion = promo;
            moves.push_back(m);
          }
        } else {
          for (auto promo : {QUEEN_B, ROOK_B, BISHOP_B, KNIGHT_B}) {
            Move m = addMove(board,from,to,PAWN_B);
            m.promotion = promo;
            moves.push_back(m);
          }
        }
      } else {
        moves.push_back(addMove(board,from,to, whiteToPlay?PAWN_W:PAWN_B));
      }
    };
    if (whiteToPlay) {
      BitBoard pw = board.white_pawn;
      while(pw) {
        int sq = __builtin_ctzll(pw);
        int one = sq-8;
        if (one>=0 && !(board.board_occupancy & (1ULL<<one))) {
          pushOrCap(sq,one);
          if (sq>=48 && sq<=55) {
            int two = sq-16;
            if (!(board.board_occupancy & (1ULL<<two)))
              pushOrCap(sq,two);
          }
        }
        BitBoard cap = board.get_pawn_attacks(sq,true);
        while(cap) {
          int to = __builtin_ctzll(cap);
          if ((enmOcc&(1ULL<<to)) || to==board.enPassantSq)
            pushOrCap(sq,to);
          cap &= cap-1;
        }
        pw &= pw-1;
      }
    } else {
      BitBoard pb = board.black_pawn;
      while(pb) {
        int sq = __builtin_ctzll(pb);
        int one = sq+8;
        if (one<64 && !(board.board_occupancy & (1ULL<<one))) {
          pushOrCap(sq,one);
          if (sq>=8 && sq<=15) {
            int two = sq+16;
            if (!(board.board_occupancy & (1ULL<<two)))
              pushOrCap(sq,two);
          }
        }
        BitBoard cap = board.get_pawn_attacks(sq,false);
        while(cap) {
          int to = __builtin_ctzll(cap);
          if ((enmOcc&(1ULL<<to)) || to==board.enPassantSq)
            pushOrCap(sq,to);
          cap &= cap-1;
        }
        pb &= pb-1;
      }
    }
    struct PieceGen { BitBoard bb; Pieces pt; 
      std::function<BitBoard(int)> attacks;
      BitBoard occ; };
    PieceGen gens[] = {
      { whiteToPlay?board.white_knight:board.black_knight,
        whiteToPlay?KNIGHT_W:KNIGHT_B,
        [&](int s){ return board.get_knight_attacks(s);}, 0},
      { whiteToPlay?board.white_bishop:board.black_bishop,
        whiteToPlay?BISHOP_W:BISHOP_B,
        [&](int s){ return board.get_bishop_attacks(s, board.board_occupancy);}, 0},
      { whiteToPlay?board.white_rook:board.black_rook,
        whiteToPlay?ROOK_W:ROOK_B,
        [&](int s){ return board.get_rook_attacks(s, board.board_occupancy);}, 0},
      { whiteToPlay?board.white_queen:board.black_queen,
        whiteToPlay?QUEEN_W:QUEEN_B,
        [&](int s){ return board.get_queen_attacks(s, board.board_occupancy);}, 0},
      { whiteToPlay?board.white_king:board.black_king,
        whiteToPlay?KING_W:KING_B,
        [&](int s){ return board.get_king_attacks(s);}, 0},
    };
    for (auto &g : gens) {
      auto bb = g.bb;
      while(bb) {
        int sq = __builtin_ctzll(bb);
        auto att = g.attacks(sq);
        while(att) {
          int to = __builtin_ctzll(att);
          if (!(ownOcc & (1ULL<<to)))
            moves.push_back(addMove(board,sq,to,g.pt));
          att &= att-1;
        }
        bb &= bb-1;
      }
    }
    int ks = whiteToPlay?1:4, qs=whiteToPlay?2:8;
    int start = whiteToPlay?60:4;
    if ((board.castling&ks)
      && !board.isSquareAttacked(start,!whiteToPlay)
      && !(board.board_occupancy & (1ULL<<(start+1)))
      && !board.isSquareAttacked(start+1,!whiteToPlay)
      && !(board.board_occupancy & (1ULL<<(start+2)))
      && !board.isSquareAttacked(start+2,!whiteToPlay))
    {
      Move c = addMove(board,start,start+2, gens[4].pt);
      c.castle = ks; moves.push_back(c);
    }
    if ((board.castling&qs)
      && !board.isSquareAttacked(start,!whiteToPlay)
      && !(board.board_occupancy & (1ULL<<(start-1)))
      && !board.isSquareAttacked(start-1,!whiteToPlay)
      && !(board.board_occupancy & (1ULL<<(start-2)))
      && !board.isSquareAttacked(start-2,!whiteToPlay))
    {
      Move c = addMove(board,start,start-2, gens[4].pt);
      c.castle = qs; moves.push_back(c);
    }

    return moves;
}

std::vector<Move> SearchEngine::filterLegalMoves(Board& board,
                                                 const std::vector<Move>& moves) {
  std::vector<Move> legal;
  // remember who was to move
  int movingColor = board.turn;  // +1 = White, -1 = Black

  for (auto m : moves) {
      Board copy = board;
      copy.implementMove(&m);
      // if after the move YOUR king is not in check, it’s legal
      if (!copy.isKingInCheck(movingColor))
          legal.push_back(m);
  }
  return legal;
}

void SearchEngine::populateBestMoveMinimaxSearch(Board* boardPtr) {
    auto start = std::chrono::steady_clock::now();
    Move bestMove;
    auto rootPseudo = generateLegalMoves(*boardPtr);
    auto rootLegal  = filterLegalMoves(*boardPtr, rootPseudo);
    if (rootLegal.empty()) {
    std::cout << "bestmove 0000\n";
    return;
    }
    bestMove = rootLegal[0];
    for (int d = 1; d <= searchDepth; ++d) {
        int alpha = std::numeric_limits<int>::min()/2;
        int beta  = std::numeric_limits<int>::max()/2;
        negamax(*boardPtr, d, alpha, beta, bestMove, 0);
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()
            > timeLimitMs) break;
    }
    std::cout << "bestmove " << moveToUCI(bestMove) << std::endl;
}

int SearchEngine::negamax(Board& board, int depth, int alpha, int beta, Move& best, int ply) {
    BitBoard key = board.board_hash;
    int origAlpha = alpha;
    auto moves = generateLegalMoves(board);
    moves = filterLegalMoves(board, moves);
    if (moves.empty()) {
        if (board.isKingInCheck(board.turn))
            return -10000 + ply;
            else
            return 0;  
    }
    if (depth == 0) {
        if (board.isKingInCheck(-board.turn) && filterLegalMoves(board, generateLegalMoves(board)).empty())
            return +10000;
        return board.basic_evaluate();
    }
    auto it = ttTable.find(key);
    if (it!=ttTable.end() && it->second.depth>=depth) {
      auto &e = it->second;
      if (e.nodeType==EXACT)   return e.eval;
      if (e.nodeType==LOWERBOUND) alpha = std::max(alpha,e.eval);
      if (e.nodeType==UPPERBOUND) beta  = std::min(beta ,e.eval);
      if (alpha>=beta)          return e.eval;
    }

    if (depth==0) {
      return evalEngine.basic_evaluate(
        board.white_pawn, board.black_pawn,
        board.white_knight, board.black_knight,
        board.white_bishop, board.black_bishop,
        board.white_rook, board.black_rook,
        board.white_queen, board.black_queen
      );
    }
    if (moves.empty()) {
      std::vector<Move> dummy;
      return board.getResult(dummy);
    }

    if (it!=ttTable.end()) {
      auto &tm = it->second.move;
      for (size_t i=0;i<moves.size();++i)
        if (moves[i].fromSquare==tm.fromSquare
         && moves[i].toSquare  ==tm.toSquare) {
          std::swap(moves[0],moves[i]);
          break;
        }
    }
    std::vector<Move> caps, quiets;
    for (auto &m : moves) {
      bool isCap = false;
      if (board.turn==1) {
        BitBoard mask = 1ULL<<m.toSquare;
        if (board.black_pawn & mask ||
            board.black_knight & mask ||
            board.black_bishop & mask ||
            board.black_rook & mask ||
            board.black_queen & mask ||
            board.black_king & mask)
          isCap = true;
      } else {
        BitBoard mask = 1ULL<<m.toSquare;
        if (board.white_pawn & mask ||
            board.white_knight & mask ||
            board.white_bishop & mask ||
            board.white_rook & mask ||
            board.white_queen & mask ||
            board.white_king & mask)
          isCap = true;
      }
      if (isCap) caps.push_back(m);
      else       quiets.push_back(m);
    }
    std::sort(caps.begin(), caps.end(), [&](auto &a, auto &b){
      int va = pieceValue(
        board.turn==1
          ? ( [&,to=a.toSquare](){
              if (board.black_queen  &1ULL<<to) return QUEEN_B;
              if (board.black_rook   &1ULL<<to) return ROOK_B;
              if (board.black_bishop &1ULL<<to) return BISHOP_B;
              if (board.black_knight &1ULL<<to) return KNIGHT_B;
              return PAWN_B; }() )
          : ( [&,to=a.toSquare](){
              if (board.white_queen  &1ULL<<to) return QUEEN_W;
              if (board.white_rook   &1ULL<<to) return ROOK_W;
              if (board.white_bishop &1ULL<<to) return BISHOP_W;
              if (board.white_knight &1ULL<<to) return KNIGHT_W;
              return PAWN_W; }() )
      );
      int vb = pieceValue(
        board.turn==1
          ? ( [&,to=b.toSquare](){
              if (board.black_queen  &1ULL<<to) return QUEEN_B;
              if (board.black_rook   &1ULL<<to) return ROOK_B;
              if (board.black_bishop &1ULL<<to) return BISHOP_B;
              if (board.black_knight &1ULL<<to) return KNIGHT_B;
              return PAWN_B; }() )
          : ( [&,to=b.toSquare](){
              if (board.white_queen  &1ULL<<to) return QUEEN_W;
              if (board.white_rook   &1ULL<<to) return ROOK_W;
              if (board.white_bishop &1ULL<<to) return BISHOP_W;
              if (board.white_knight &1ULL<<to) return KNIGHT_W;
              return PAWN_W; }() )
      );
      int aa = pieceValue(a.pieceType);
      int ab = pieceValue(b.pieceType);
      return (va*100 - aa) > (vb*100 - ab);
    });
    for (int k=0;k<2;k++) {
      auto km = killers[ply][k];
      if (km.fromSquare>=0) {
        auto it = std::find_if(quiets.begin(),quiets.end(),[&](auto &m){
          return m.fromSquare==km.fromSquare && m.toSquare==km.toSquare;
        });
        if (it!=quiets.end()) {
          quiets.erase(it);
          quiets.insert(quiets.begin(),km);
        }
      }
    }
    std::sort(quiets.begin(),quiets.end(),[&](auto &a, auto &b){
      return historyScores[a.fromSquare][a.toSquare]
           > historyScores[b.fromSquare][b.toSquare];
    });
    moves.clear();
    moves.insert(moves.end(),caps.begin(),caps.end());
    moves.insert(moves.end(),quiets.begin(),quiets.end());
    int bestVal = std::numeric_limits<int>::min()/2;
    Move   bestMoveLocal{};
    for (auto &m : moves) {
      Board child = board;
      child.implementMove(&m);
      Move dummy;
      int val = -negamax(child, depth-1, -beta, -alpha, dummy, ply+1);

      if (val > bestVal) { bestVal=val; bestMoveLocal=m; }
      if (val > alpha) {
        if (!(board.turn==1?
             ((board.black_pawn  | board.black_knight |
               board.black_bishop| board.black_rook    |
               board.black_queen | board.black_king) & (1ULL<<m.toSquare))
             :
             ((board.white_pawn  | board.white_knight |
               board.white_bishop| board.white_rook    |
               board.white_queen | board.white_king) & (1ULL<<m.toSquare))
            )) {
          historyScores[m.fromSquare][m.toSquare] += depth*depth;
        }
        alpha = val;
      }
      if (alpha >= beta) {
        if (!(it!=ttTable.end() &&
              it->second.move.fromSquare==m.fromSquare &&
              it->second.move.toSquare  ==m.toSquare))
        {
          killers[ply][1] = killers[ply][0];
          killers[ply][0] = m;
        }
        break;
      }
    }
    TTEntry ne{ key, bestVal, 0, depth, bestMoveLocal };
    if (bestVal <= origAlpha) ne.nodeType = UPPERBOUND;
    else if (bestVal >= beta) ne.nodeType = LOWERBOUND;
    else                      ne.nodeType = EXACT;
    ttTable[key]=ne;

    best = bestMoveLocal;
    return bestVal;
}

std::string SearchEngine::moveToUCI(const Move& m) {
    std::string s;
    s.push_back('a' + (m.fromSquare % 8));
    s.push_back('1' + (7 - m.fromSquare / 8));
    s.push_back('a' + (m.toSquare   % 8));
    s.push_back('1' + (7 - m.toSquare   / 8));
    // fromSquare → file/rank in exactly the inverse of populateMove:
  /*int file_from =  m.fromSquare % 8;
  int rank_from = 7 - (m.fromSquare / 8);
  int file_to   =  m.toSquare   % 8;
  int rank_to   = 7 - (m.toSquare   / 8);
  s.push_back(char('a' + file_from));
  s.push_back(char('1' + rank_from));
  s.push_back(char('a' + file_to));
  s.push_back(char('1' + rank_to));*/
    if ((m.pieceType == PAWN_W || m.pieceType == PAWN_B)
        && m.promotion  != m.pieceType) {
    char promoChar = 'q';
    switch (m.promotion & 1) {
        case KING_W: case KING_B:   promoChar='k'; break;
        case QUEEN_W:case QUEEN_B:  promoChar='q'; break;
        case ROOK_W: case ROOK_B:   promoChar='r'; break;
        case BISHOP_W:case BISHOP_B: promoChar='b'; break;
        case KNIGHT_W:case KNIGHT_B: promoChar='n'; break;
    }
    s.push_back(promoChar);
    }
    return s;
}
