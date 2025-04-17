#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <cstdint>

typedef uint64_t BitBoard;
enum Pieces{
    PAWN_W, PAWN_B, KNIGHT_W, KNIGHT_B, ROOK_W, ROOK_B, BISHOP_W, BISHOP_B, QUEEN_W, QUEEN_B, KING_W,KING_B
};
typedef struct {
    int fromSquare;
    int toSquare;
    Pieces promotion;
    int castle;
    int validation;
    Pieces pieceType;
    int score;
    bool exhausted;
} Move;
#endif