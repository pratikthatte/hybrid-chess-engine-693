#ifndef MOVE_H
#define MOVE_H

#include <array>
#include "typedefs.h"

class MoveGenerationEngine {
public:
    MoveGenerationEngine();
    ~MoveGenerationEngine();
    BitBoard get_knight_attacks(int square);
    BitBoard get_king_attacks(int square);
    BitBoard get_pawn_attacks(int square, bool is_white);
    BitBoard get_bishop_attacks(int square, BitBoard occupied);
    BitBoard get_rook_attacks(int square, BitBoard occupied);
    BitBoard get_queen_attacks(int square, BitBoard occupied);

private:
    std::array<BitBoard, 64> knight_movement;
    std::array<BitBoard, 64> king_movement;
    std::array<BitBoard, 64> white_pawn_left_movement;
    std::array<BitBoard, 64> white_pawn_right_movement;
    std::array<BitBoard, 64> black_pawn_left_movement;
    std::array<BitBoard, 64> black_pawn_right_movement;
    std::array<BitBoard, 64> bishop_movement;
    std::array<BitBoard, 64> rook_movement;
    std::array<int, 64> bishop_number_of_reachable_squares;
    std::array<int, 64> rook_number_of_reachable_squares;
    std::array<BitBoard, 64> bishop_magic_square_values;
    std::array<BitBoard, 64> rook_magic_square_values;
    std::array<std::array<BitBoard, 512>, 64> bishop_attack_masks;
    std::array<std::array<BitBoard, 4096>, 64> rook_attack_masks;
    void generate_knight_movement();
    void generate_king_movement();
    void generate_pawn_movement();
    void generate_bishop_movement();
    void generate_rook_movement();
    void generate_obstacle_based_bishop_movement();
    void generate_obstacle_based_rook_movement();
    BitBoard generate_obstacle_map(int current_obstacle_combination, 
                                  int number_of_bits, 
                                  BitBoard future_moves);
    BitBoard generate_bishop_attacks_with_obstacles(int sq, BitBoard obstacle_map);
    BitBoard generate_rook_attacks_with_obstacles(int sq, BitBoard obstacle_map);
};
#endif