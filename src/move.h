#ifndef MOVE_H
#define MOVE_H

#include <array>
#include <vector>
#include "typedefs.h"
#include <string>

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
    BitBoard get_pawn_white_right(int sq);
    BitBoard get_pawn_white_left(int sq);
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
    std::vector<std::vector<BitBoard>> bishop_attack_masks;
    std::vector<std::vector<BitBoard>> rook_attack_masks;
    void generate_knight_movement();
    void generate_king_movement();
    void generate_pawn_movement();
    void generate_bishop_movement();
    void generate_rook_movement();
    void generate_obstacle_based_bishop_movement();
    void generate_obstacle_based_rook_movement();
    BitBoard generate_obstacle_map(int current_obstacle_combination, int number_of_bits, BitBoard future_moves);
    BitBoard generate_bishop_attacks_with_obstacles(int sq, BitBoard obstacle_map);
    BitBoard generate_rook_attacks_with_obstacles(int sq, BitBoard obstacle_map);
    void save_rook_masks_to_file(const std::string& filename);
    bool load_rook_masks_from_file(const std::string& filename);
    void save_bishop_masks_to_file(const std::string& filename);
    bool load_bishop_masks_from_file(const std::string& filename);
};

#endif
