#include "search.h"
#include "evaluation.h"
#include "move.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <vector>

SearchEngine::SearchEngine() { }
SearchEngine::~SearchEngine() { }

int SearchEngine::evaluateBoard(Board &board) {
    return board.basic_evaluate(
        board.white_pawn, board.black_pawn,
        board.white_knight, board.black_knight,
        board.white_bishop, board.black_bishop,
        board.white_rook, board.black_rook,
        board.white_queen, board.black_queen
    );
}

std::vector<Move> SearchEngine::generateLegalMoves(Board &board) {
    std::vector<Move> moves;
    BitBoard ownOccupancy = board.turn==1 ? board.board_occupancy_white : board.board_occupancy_black;
    BitBoard enemyOccupancy = board.turn==1 ? board.board_occupancy_black : board.board_occupancy_white;

    if (board.turn==1) {
        BitBoard pawns = board.white_pawn;
        while (pawns) {
            int sq = __builtin_ctzll(pawns);
            int toSq = sq - 8;
            if (toSq >= 0 && !(board.board_occupancy & (1ULL << toSq))) {
                moves.push_back(addMove(board, sq, toSq, PAWN_W));
            }
            BitBoard capBB = board.get_pawn_attacks(sq, true);
            while (capBB) {
                int capSq = __builtin_ctzll(capBB);
                if (enemyOccupancy & (1ULL << capSq)) {
                    moves.push_back(addMove(board, sq, capSq, PAWN_W));
                }
                capBB &= capBB - 1;
            }
            pawns &= pawns - 1;
        }
    } else {
        BitBoard pawns = board.black_pawn;
        while (pawns) {
            int sq = __builtin_ctzll(pawns);
            int toSq = sq + 8;
            if (toSq < 64 && !(board.board_occupancy & (1ULL << toSq))) {
                moves.push_back(addMove(board, sq, toSq, PAWN_B));
            }
            BitBoard capBB = board.get_pawn_attacks(sq, false);
            while (capBB) {
                int capSq = __builtin_ctzll(capBB);
                if (enemyOccupancy & (1ULL << capSq)) {
                    moves.push_back(addMove(board,sq, capSq, PAWN_B));
                }
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

    return moves;
}

int SearchEngine::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    std::vector<Move> legalMoves = generateLegalMoves(board);
    if (depth == 0 || legalMoves.empty())
        return evaluateBoard(board);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (Move move : legalMoves) {
            Board boardCopy = board;
            boardCopy.implementMove(&move);
            int eval = minimax(boardCopy, depth - 1, alpha, beta, false);
            if (eval > maxEval)
                maxEval = eval;
            if (maxEval > alpha)
                alpha = maxEval;
            if (beta <= alpha)
                break;
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (const Move &move : legalMoves) {
            Board boardCopy = board;
            boardCopy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(boardCopy, depth - 1, alpha, beta, true);
            if (eval < minEval)
                minEval = eval;
            if (minEval < beta)
                beta = minEval;
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

void SearchEngine::populateBestMoveMinimaxSearch(Board* board) {
    std::vector<Move> legalMoves = generateLegalMoves(*board);
    if (legalMoves.empty()) {
        printf("bestmove (none)\n");
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
}
Move SearchEngine::addMove(Board &board, int fromSq, int toSq, Pieces piece) {
    Move m;
    m.fromSquare = fromSq;
    m.toSquare = toSq;
    m.pieceType = piece;
    m.castle = 0;
    if(piece == (board.turn==1 ? PAWN_W : PAWN_B)) {
        if (board.turn==1 && toSq < 8)
            m.promotion = QUEEN_W;
        else if (board.turn!=1 && toSq >= 56)
            m.promotion = QUEEN_B;
        else
            m.promotion = piece;
    } else {
        m.promotion = piece;
    }
    return m;
};
