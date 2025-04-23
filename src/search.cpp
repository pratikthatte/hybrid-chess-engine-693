#include "search.h"
#include "evaluation.h"
#include "move.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
enum NodeType {
    EXACT = 0,
    LOWERBOUND = 1,
    UPPERBOUND = 2
};
SearchEngine::SearchEngine(EvaluationEngine& evaluationEngine):evaluationEngine(evaluationEngine){ }
SearchEngine::~SearchEngine() { }

int SearchEngine::evaluateBoard(Board &board) {
    return board.basic_evaluate();
}
int SearchEngine::scoreMove(const Move& move) {
    int score = 0;
    if (move.promotion == QUEEN_W || move.promotion == QUEEN_B)
        score += 1000;

    int to = move.toSquare;

    switch (move.pieceType) {
        case PAWN_W:   score += 10 + evaluationEngine.white_pawn_position_values[to]; break;
        case KNIGHT_W: score += 30 + evaluationEngine.white_knight_position_values[to]; break;
        case BISHOP_W: score += 35 + evaluationEngine.white_bishop_position_values[to]; break;
        case ROOK_W:   score += 50 + evaluationEngine.white_rook_position_values[to]; break;
        case QUEEN_W:  score += 90 + evaluationEngine.white_queen_position_values[to]; break;
        case KING_W:   score += evaluationEngine.white_king_position_values[to]; break;

        case PAWN_B:   score += 10 + evaluationEngine.black_pawn_position_values[to]; break;
        case KNIGHT_B: score += 30 + evaluationEngine.black_knight_position_values[to]; break;
        case BISHOP_B: score += 35 + evaluationEngine.black_bishop_position_values[to]; break;
        case ROOK_B:   score += 50 + evaluationEngine.black_rook_position_values[to]; break;
        case QUEEN_B:  score += 90 + evaluationEngine.black_queen_position_values[to]; break;
        case KING_B:   score += evaluationEngine.black_king_position_values[to]; break;
        default: break;
    }

    return score;
}
void SearchEngine::orderMoves(std::vector<Move>& moves) {
    std::shuffle(moves.begin(), moves.end(), std::mt19937(std::random_device{}()));
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return scoreMove(a) > scoreMove(b);
    });
}
std::vector<Move> SearchEngine::generateLegalMoves(Board &board) {
    std::vector<Move> moves;
    BitBoard ownOccupancy = board.turn==1 ? board.board_occupancy_white : board.board_occupancy_black;
    BitBoard enemyOccupancy = board.turn==1 ? board.board_occupancy_black : board.board_occupancy_white;

    if (board.turn == 1) {
        BitBoard pawns = board.white_pawn;
        while (pawns) {
            int sq = __builtin_ctzll(pawns);
            int oneStep = sq - 8; // Move one rank forward (remember: lower index = moving up for White).
            if (oneStep >= 0 && !(board.board_occupancy & (1ULL << oneStep))) {
                moves.push_back(addMove(board, sq, oneStep, PAWN_W));
                // Pawn double move: if pawn is on starting rank (row 6: indices 48-55).
                if (sq >= 48 && sq <= 55) {
                    int twoStep = sq - 16;
                    if (twoStep >= 0 && !(board.board_occupancy & (1ULL << twoStep)))
                        moves.push_back(addMove(board, sq, twoStep, PAWN_W));
                }
            }
            // Generate pawn captures (including en passant):
            BitBoard capBB = board.get_pawn_attacks(sq, true);
            while (capBB) {
                int capSq = __builtin_ctzll(capBB);
                // Normal capture if enemy occupies target:
                if (board.turn == 1 && (board.board_occupancy & (1ULL << capSq)) &&
                    (board.board_occupancy & board.board_occupancy_black))
                    moves.push_back(addMove(board, sq, capSq, PAWN_W));
                // En passant capture: if target square is the en passant square.
                else if (capSq == board.enPassantSq)
                    moves.push_back(addMove(board, sq, capSq, PAWN_W));
                capBB &= capBB - 1;
            }
            pawns &= pawns - 1;
        }
    }
    if (board.turn == -1) {
        BitBoard pawns = board.black_pawn;
        while (pawns) {
            int sq = __builtin_ctzll(pawns);
            int oneStep = sq + 8; // For black, moving "down" increases the index.
            if (oneStep < 64 && !(board.board_occupancy & (1ULL << oneStep))) {
                moves.push_back(addMove(board, sq, oneStep, PAWN_B));
                // Pawn double move: if pawn is on starting rank (row 1: indices 8-15).
                if (sq >= 8 && sq <= 15) {
                    int twoStep = sq + 16;
                    if (twoStep < 64 && !(board.board_occupancy & (1ULL << twoStep)))
                        moves.push_back(addMove(board, sq, twoStep, PAWN_B));
                }
            }
            BitBoard capBB = board.get_pawn_attacks(sq, false);
            while (capBB) {
                int capSq = __builtin_ctzll(capBB);
                if (capSq == board.enPassantSq)
                    moves.push_back(addMove(board, sq, capSq, PAWN_B));
                else if (board.turn == -1 && (board.board_occupancy & (1ULL << capSq)) &&
                         (board.board_occupancy & board.board_occupancy_white))
                    moves.push_back(addMove(board, sq, capSq, PAWN_B));
                capBB &= capBB - 1;
            }
            pawns &= pawns - 1;
        }
    }    
    if (board.turn==1) {
        BitBoard knights = board.white_knight;
        while (knights) {
            int sq = __builtin_ctzll(knights);
            BitBoard attBB = board.get_knight_attacks(sq);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, KNIGHT_W));
                }
                attBB &= attBB - 1;
            }
            knights &= knights - 1;
        }
    } else {
        BitBoard knights = board.black_knight;
        while (knights) {
            int sq = __builtin_ctzll(knights);
            BitBoard attBB = board.get_knight_attacks(sq);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, KNIGHT_B));
                }
                attBB &= attBB - 1;
            }
            knights &= knights - 1;
        }
    }
    if (board.turn==1) {
        BitBoard bishops = board.white_bishop;
        while (bishops) {
            int sq = __builtin_ctzll(bishops);
            BitBoard attBB = board.get_bishop_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, BISHOP_W));
                }
                attBB &= attBB - 1;
            }
            bishops &= bishops - 1;
        }
    } else {
        BitBoard bishops = board.black_bishop;
        while (bishops) {
            int sq = __builtin_ctzll(bishops);
            BitBoard attBB = board.get_bishop_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, BISHOP_B));
                }
                attBB &= attBB - 1;
            }
            bishops &= bishops - 1;
        }
    }

    if (board.turn==1) {
        BitBoard rooks = board.white_rook;
        while (rooks) {
            int sq = __builtin_ctzll(rooks);
            BitBoard attBB = board.get_rook_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, ROOK_W));
                }
                attBB &= attBB - 1;
            }
            rooks &= rooks - 1;
        }
    } else {
        BitBoard rooks = board.black_rook;
        while (rooks) {
            int sq = __builtin_ctzll(rooks);
            BitBoard attBB = board.get_rook_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, ROOK_B));
                }
                attBB &= attBB - 1;
            }
            rooks &= rooks - 1;
        }
    }
    if (board.turn==1) {
        BitBoard queens = board.white_queen;
        while (queens) {
            int sq = __builtin_ctzll(queens);
            BitBoard attBB = board.get_queen_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, QUEEN_W));
                }
                attBB &= attBB - 1;
            }
            queens &= queens - 1;
        }
    } else {
        BitBoard queens = board.black_queen;
        while (queens) {
            int sq = __builtin_ctzll(queens);
            BitBoard attBB = board.get_queen_attacks(sq, board.board_occupancy);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, QUEEN_B));
                }
                attBB &= attBB - 1;
            }
            queens &= queens - 1;
        }
    }

    if (board.turn==1) {
        BitBoard king = board.white_king;
        while (king) { 
            int sq = __builtin_ctzll(king);
            BitBoard attBB = board.get_king_attacks(sq);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, KING_W));
                }
                attBB &= attBB - 1;
            }
            king &= king - 1;
        }
    } else {
        BitBoard king = board.black_king;
        while (king) {
            int sq = __builtin_ctzll(king);
            BitBoard attBB = board.get_king_attacks(sq);
            while (attBB) {
                int toSq = __builtin_ctzll(attBB);
                if (!(ownOccupancy & (1ULL << toSq))) {
                    moves.push_back(addMove(board,sq, toSq, KING_B));
                }
                attBB &= attBB - 1;
            }
            king &= king - 1;
        }
    }
    if (board.turn == 1) {
        // White king castling. Assume white king initial square is 60.
        if (board.white_king & (1ULL << 60)) {
            // Kingside castling: check that squares 61 and 62 are empty.
            if ((board.board_occupancy & (1ULL << 61)) == 0 &&
                (board.board_occupancy & (1ULL << 62)) == 0 &&
                (board.castling & 1)) {  // white kingside castling right
                // Optionally: verify squares 61 and 62 are not under attack.
                Move castleMove = addMove(board, 60, 62, KING_W);
                castleMove.castle = 1;  // use 1 to denote white kingside castle
                moves.push_back(castleMove);
            }
            // Queenside castling: check that squares 59, 58, (and 57 if required) are empty.
            if ((board.board_occupancy & (1ULL << 59)) == 0 &&
                (board.board_occupancy & (1ULL << 58)) == 0 &&
                (board.board_occupancy & (1ULL << 57)) == 0 &&
                (board.castling & 2)) {  // white queenside castling right
                // Optionally: verify squares 59 and 58 are not under attack.
                Move castleMove = addMove(board, 60, 58, KING_W);
                castleMove.castle = 2;  // use 2 to denote white queenside castle
                moves.push_back(castleMove);
            }
        }
    } else {
        // Black king castling. Assume black king initial square is 4.
        if (board.black_king & (1ULL << 4)) {
            if ((board.board_occupancy & (1ULL << 5)) == 0 &&
                (board.board_occupancy & (1ULL << 6)) == 0 &&
                (board.castling & 4)) {  // black kingside castling right
                Move castleMove = addMove(board, 4, 6, KING_B);
                castleMove.castle = 4; // denote black kingside castle
                moves.push_back(castleMove);
            }
            if ((board.board_occupancy & (1ULL << 3)) == 0 &&
                (board.board_occupancy & (1ULL << 2)) == 0 &&
                (board.board_occupancy & (1ULL << 1)) == 0 &&
                (board.castling & 8)) {  // black queenside castling right
                Move castleMove = addMove(board, 4, 2, KING_B);
                castleMove.castle = 8; // denote black queenside castle
                moves.push_back(castleMove);
            }
        }
    }    
    std::cout << "[DEBUG] Generated " << moves.size() << " legal moves\n";
    int knightCount = 0, bishopCount = 0, rookCount = 0, queenCount = 0, kingCount = 0;
    for (const auto& m : moves) {
        if (m.pieceType == KNIGHT_W || m.pieceType == KNIGHT_B) knightCount++;
        if (m.pieceType == BISHOP_W || m.pieceType == BISHOP_B) bishopCount++;
        if (m.pieceType == ROOK_W || m.pieceType == ROOK_B) rookCount++;
        if (m.pieceType == QUEEN_W || m.pieceType == QUEEN_B) queenCount++;
        if (m.pieceType ==KING_W || m.pieceType == KING_B)  kingCount++;
    }
std::cout << "[DEBUG] Knight: " << knightCount << ", Bishop: " << bishopCount
          << ", Rook: " << rookCount << ", Queen: " << queenCount << ", King: "<<kingCount<< std::endl;
    return moves;
}

int SearchEngine::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    BitBoard hash = board.board_hash;

    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end()) {
        const TTEntry& entry = it->second;
        if (entry.depth >= depth) {
            if (entry.nodeType == EXACT) return entry.eval;
            if (entry.nodeType == LOWERBOUND && entry.eval >= beta) return entry.eval;
            if (entry.nodeType == UPPERBOUND && entry.eval <= alpha) return entry.eval;
        }
    }

    std::vector<Move> pseudoLegalMoves = generateLegalMoves(board);
    std::vector<Move> legalMoves = filterLegalMoves(board,pseudoLegalMoves);
    if (depth == 0 || legalMoves.empty())
        return evaluateBoard(board);

    int originalAlpha = alpha;
    int bestEval;

    if (maximizingPlayer) {
        bestEval = INT_MIN;
        for (const Move &move : legalMoves) {
            Board copy = board;
            copy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(copy, depth - 1, alpha, beta, false);
            bestEval = std::max(bestEval, eval);
            alpha = std::max(alpha, bestEval);
            if (beta <= alpha) break;
        }
    } else {
        bestEval = INT_MAX;
        for (const Move &move : legalMoves) {
            Board copy = board;
            copy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(copy, depth - 1, alpha, beta, true);
            bestEval = std::min(bestEval, eval);
            beta = std::min(beta, bestEval);
            if (beta <= alpha) break;
        }
    }

    TTEntry entry;
    entry.zobrist = hash;
    entry.eval = bestEval;
    entry.depth = depth;
    entry.nodeType = (bestEval <= originalAlpha) ? UPPERBOUND :
                     (bestEval >= beta) ? LOWERBOUND : EXACT;

    transpositionTable[hash] = entry;
    return bestEval;
}
void SearchEngine::populateBestMoveMinimaxSearch(Board* board) {
    std::vector<Move> pseudoLegalMoves = generateLegalMoves(*board);
    std::vector<Move> legalMoves = filterLegalMoves(*board,pseudoLegalMoves);
    if (legalMoves.empty()) {
        printf("bestmove (none)\n");
        fflush(stdout);
        return;
    }

    int bestValue = INT_MIN;
    Move bestMove = legalMoves[0];
    for (const Move &move : legalMoves) {
        Board boardCopy = *board;
        boardCopy.implementMove(const_cast<Move*>(&move));
        int eval = minimax(boardCopy, SEARCH_DEPTH - 1, INT_MIN, INT_MAX, false);
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = move;
        }
    }

    std::vector<Move> psuedoConfirmMoves = generateLegalMoves(*board);
    std::vector<Move> confirmMoves = filterLegalMoves(*board,psuedoConfirmMoves);
    bool valid = false;
    for (const Move& m : confirmMoves) {
        if (m.fromSquare == bestMove.fromSquare && m.toSquare == bestMove.toSquare) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        printf("bestmove (none)\n");
        fflush(stdout);
        return;
    }

    char moveStr[6];
    int fromCol = bestMove.fromSquare % 8;
    int fromRow = bestMove.fromSquare / 8;
    int toCol = bestMove.toSquare % 8;
    int toRow = bestMove.toSquare / 8;
    moveStr[0] = 'a' + fromCol;
    moveStr[1] = '8' - fromRow;
    moveStr[2] = 'a' + toCol;
    moveStr[3] = '8' - toRow;
    if ((bestMove.pieceType == PAWN_W && bestMove.toSquare < 8) ||
        (bestMove.pieceType == PAWN_B && bestMove.toSquare >= 56)) {
        moveStr[4] = 'q';
        moveStr[5] = '\0';
    } else {
        moveStr[4] = '\0';
    }
    printf("bestmove %s\n", moveStr);
    fflush(stdout);
}
Move SearchEngine::addMove(Board &board, int fromSq, int toSq, Pieces piece) {
    Move m;
    m.fromSquare = fromSq;
    m.toSquare = toSq;
    m.pieceType = piece;
    m.castle = 0;
    if (piece == PAWN_W && toSq < 8)
        m.promotion = QUEEN_W;
    else if (piece == PAWN_B && toSq >= 56)
        m.promotion = QUEEN_B;
    else
        m.promotion = piece;
    return m;
}
double SearchEngine::uctValue(TreeNode* child, int parentVisits, double C = 1.4){
    if (child->visits == 0) return std::numeric_limits<double>::max();
    return child->totalValue / child->visits + C * std::sqrt(std::log(parentVisits) / child->visits);
}
SearchEngine::TreeNode* SearchEngine::selectChild(TreeNode* node){
    TreeNode* bestChild = nullptr;
    double bestVal = -std::numeric_limits<double>::infinity();
    for (TreeNode* child : node->children) {
        double val = uctValue(child, node->visits);
        if (val > bestVal) {
            bestVal = val;
            bestChild = child;
        }
    }
    return bestChild;
}
int SearchEngine::rollout(Board simulationBoard){
        const int rolloutDepth = 10;
        for (int i = 0; i < rolloutDepth; i++) {
            std::vector<Move> pseudoMoves = this->generateLegalMoves(simulationBoard);
            std::vector<Move> moves = filterLegalMoves(simulationBoard,pseudoMoves);
            if (moves.empty())
                break;
            int idx = std::rand() % moves.size();
            simulationBoard.implementMove(const_cast<Move*>(&moves[idx]));
        }
        return simulationBoard.basic_evaluate();
}
void SearchEngine::backpropagate(TreeNode* node, double result){
    while (node) {
        node->visits++;
        node->totalValue += result;
        node = node->parent;
    }
}
void SearchEngine::populateBestMoveMCTSSearch(Board* board){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    TreeNode* root = new TreeNode(*board);
    std::vector<Move> pseudoUntriedMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board,pseudoUntriedMoves);
    const int ITERATIONS = 1000;
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            std::vector<Move> psuedoChildUntriedMoves= generateLegalMoves(*simulationBoard);
             child->untriedMoves = filterLegalMoves(*simulationBoard,psuedoChildUntriedMoves);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout(*simulationBoard);
        backpropagate(node, result);
    }
    TreeNode* bestChild = nullptr;
    int bestVisits = -1;
    for (TreeNode* child : root->children) {
        if (child->visits > bestVisits) {
            bestVisits = child->visits;
            bestChild = child;
        }
    }

    if (bestChild) {
        int fromCol = bestChild->move.fromSquare % 8;
        int fromRow = bestChild->move.fromSquare / 8;
        int toCol = bestChild->move.toSquare % 8;
        int toRow = bestChild->move.toSquare / 8;
        char moveStr[6];
        moveStr[0] = 'a' + fromCol;
        moveStr[1] = '8' - fromRow;
        moveStr[2] = 'a' + toCol;
        moveStr[3] = '8' - toRow;
        moveStr[4] = '\0';
        printf("bestmove %s\n", moveStr);
    } else {
        printf("bestmove (none)\n");
    }
    delete root;
}
void SearchEngine::populateBestMoveMCTS_IR_M(Board* board){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    TreeNode* root = new TreeNode(*board);
    std::vector<Move> psuedoUntriedMoves= generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board,psuedoUntriedMoves);

    const int ITERATIONS = 10;
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            std::vector<Move> pseudoChildUntriedMoves = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard,pseudoChildUntriedMoves);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout_IR_M(*simulationBoard);
        backpropagate(node, result);
    }
    printBestMoveFromRoot(root);
    delete root;
}
double SearchEngine::rollout_IR_M(Board board) {
    int rolloutDepth = 3;
    for (int i = 0; i < rolloutDepth; i++) {
        std::vector<Move> pseudoLegalMoves = generateLegalMoves(board);
        std::vector<Move> legalMoves = filterLegalMoves(board,pseudoLegalMoves);
        if (legalMoves.empty()) break;
        Move move = selectMoveByMinimax(board, 1);
        board.implementMove(&move);
    }
    for (int i = 0; i < 10; i++) {
        std::vector<Move> pseudoLegalMoves = generateLegalMoves(board);
        std::vector<Move> legalMoves = filterLegalMoves(board,pseudoLegalMoves);
        if (legalMoves.empty()) break;
        orderMoves(legalMoves);
        board.implementMove(&legalMoves[0]);
    }

    return normalizeEvaluation(board.basic_evaluate(), -1000, 1000);
}
double SearchEngine::rollout_IC_M(Board board, int rolloutMoves = 3) {
    for (int i = 0; i < rolloutMoves; i++) {
        std::vector<Move> pseudoLegal = generateLegalMoves(board);
        std::vector<Move> legal = filterLegalMoves(board,pseudoLegal);
        if (legal.empty()) break;
        orderMoves(legal);
        board.implementMove(&legal[0]);
    }
    std::vector<Move> pseudoMoves = generateLegalMoves(board);
    std::vector<Move> moves = filterLegalMoves(board,pseudoMoves);
    if (moves.empty()) {
        return board.getResult(moves);
    } else {
        int eval = minimax(board, 1, INT_MIN, INT_MAX, true);
        return normalizeEvaluation(eval,-1000,1000);
    }
}
void SearchEngine::populateBestMoveMCTS_IC_M(Board* board) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    TreeNode* root = new TreeNode(*board);

    std::vector<Move> pseudoMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board,pseudoMoves);

    const int ITERATIONS = 10;
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            std::vector<Move> pseudoUntriedChildMoves = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard,pseudoUntriedChildMoves);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout_IC_M(*simulationBoard);
        backpropagate(node, result);
    }
    printBestMoveFromRoot(root);
    delete root;
}
void SearchEngine::populateBestMoveMCTS_IP_M(Board* board){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    TreeNode* root = new TreeNode(*board);

    std::vector<Move> pseudoMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board, pseudoMoves);
    initializeNodeWithMinimax(root, 1, 100);
    int allowedMs = (timeLimitMs > 0 ? timeLimitMs : 300000);
    auto start = std::chrono::steady_clock::now();

    int iterations = 0;
    while (true) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= allowedMs)
            break;

        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            std::vector<Move> childPseudoMoves = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard, childPseudoMoves);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout(*simulationBoard);
        backpropagate(node, result);
        iterations++;
    }

    std::cout << "[DEBUG] MCTS_IP iterations: " << iterations << std::endl;
    printBestMoveFromRoot(root);
    delete root;
}
double SearchEngine::normalizeEvaluation(int eval, int minEval=-1000, int maxEval=1000){
    return (double)(eval - minEval) / (maxEval - minEval);
}
Move SearchEngine::selectMoveByMinimax(Board &board, int depth){
    std::vector<Move> pseudoLegalMoves = generateLegalMoves(board);
    std::vector<Move> legalMoves = filterLegalMoves(board,pseudoLegalMoves);
    if (legalMoves.empty()) return Move{};
    orderMoves(legalMoves);

    Move bestMove = legalMoves[0];
    int bestEval = INT_MIN;

    for (const Move& move : legalMoves) {
        Board copy = board;
        copy.implementMove(const_cast<Move*>(&move));
        int eval = minimax(copy, depth - 1, INT_MIN, INT_MAX, false);
        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }

    return bestMove;
}
void SearchEngine::initializeNodeWithMinimax(TreeNode* node, int depth = 2, int gamma = 100) {
    int eval = minimax(node->board, depth, INT_MIN, INT_MAX, true);
    double normEval = normalizeEvaluation(eval,-1000,1000);
    node->visits += gamma;
    node->wins += gamma * normEval;
}
SearchEngine::TreeNode* SearchEngine::expandChildWithMinimax(TreeNode* parent, Move move) {
    Board nextBoard = parent->board;
    nextBoard.implementMove(&move);
    TreeNode* child = new TreeNode(nextBoard, parent);
    child->move = move;
    std::vector<Move> pseudoChildUntriedMoves = generateLegalMoves(nextBoard);
    child->untriedMoves = filterLegalMoves(nextBoard,pseudoChildUntriedMoves);
    initializeNodeWithMinimax(child);
    parent->children.push_back(child);
    return child;
}
void SearchEngine::printBestMoveFromRoot(TreeNode* root) {
    TreeNode* bestChild = nullptr;
    int bestVisits = -1;

    for (TreeNode* child : root->children) {
        if (child->visits > bestVisits) {
            bestVisits = child->visits;
            bestChild = child;
        }
    }

    if (bestChild) {
        std::vector<Move> pseudoLegalMoves = generateLegalMoves(root->board);
        std::vector<Move> legalMoves = filterLegalMoves(root->board,pseudoLegalMoves);
        bool isValid = false;
        for (const Move& m : legalMoves) {
            if (m.fromSquare == bestChild->move.fromSquare &&
                m.toSquare == bestChild->move.toSquare) {
                isValid = true;
                break;
            }
        }

        if (!isValid) {
            printf("bestmove (none)");
            fflush(stdout);
            return;
        }

        int fromCol = bestChild->move.fromSquare % 8;
        int fromRow = bestChild->move.fromSquare / 8;
        int toCol = bestChild->move.toSquare % 8;
        int toRow = bestChild->move.toSquare / 8;
        char moveStr[6];
        moveStr[0] = 'a' + fromCol;
        moveStr[1] = '8' - fromRow;
        moveStr[2] = 'a' + toCol;
        moveStr[3] = '8' - toRow;
        moveStr[4] = '\0';

        printf("bestmove %s\n", moveStr);
        fflush(stdout);
    } else {
        printf("bestmove (none)\n");
        fflush(stdout);
    }
}
std::vector<Move> SearchEngine::filterLegalMoves(const Board &board, const std::vector<Move> &pseudoLegalMoves){
    std::vector<Move> legalMoves;
    for (const Move &move : pseudoLegalMoves) {
        Board boardCopy = board;
        boardCopy.implementMove(const_cast<Move*>(&move));
        boardCopy.generateAttackMasks();        
        if (board.turn == 1) {
            if ((boardCopy.board_attack_mask & (1ULL << boardCopy.whiteKingSq)) == 0)
                legalMoves.push_back(move);
        } else {
            if ((boardCopy.board_attack_mask & (1ULL << boardCopy.blackKingSq)) == 0)
                legalMoves.push_back(move);
        }
    }
    return legalMoves;
}
