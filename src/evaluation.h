#ifndef EVALUATION_H
#define EVALUATION_H

#include <unordered_map>
#include <array>
#include "typedefs.h"

class EvaluationEngine {
public:
    EvaluationEngine();
    ~EvaluationEngine();
    int basic_evaluate(
        BitBoard white_pawns, BitBoard black_pawns,
        BitBoard white_knights, BitBoard black_knights,
        BitBoard white_bishops, BitBoard black_bishops,
        BitBoard white_rooks, BitBoard black_rooks,
        BitBoard white_queens, BitBoard black_queens
    );

private:
    std::unordered_map<Pieces, int> piece_value_map;
    std::array<int, 64> white_pawn_position_values;
    std::array<int, 64> black_pawn_position_values;
    std::array<int, 64> white_knight_position_values;
    std::array<int, 64> black_knight_position_values;
    std::array<int, 64> white_bishop_position_values;
    std::array<int, 64> black_bishop_position_values;
    std::array<int, 64> white_rook_position_values;
    std::array<int, 64> black_rook_position_values;
    std::array<int, 64> white_queen_position_values;
    std::array<int, 64> black_queen_position_values;
    std::array<int, 64> white_king_position_values;
    std::array<int, 64> black_king_position_values;
    int calculate_number_of_pieces(BitBoard bb);
    int calculate_piece_position_values(BitBoard bb, std::array<int,64>& position_values);
};

#endif