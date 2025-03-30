#ifndef MOVE_H
#define MOVE_H

class MoveGenerationEngine(){
    public:
        MoveGenerationEngine(Board* board);
        ~MoveGenerationEngine();
    private:
        void generate_knight_movement();
        void generate_king_movement();
        void generate_pawn_movement();
        void generate_bishop_movement();
        void generate_rook_movement();
        void generate_obstacle_based_bishop_movement();
        void generate_obstacle_based_rook_movement();
        BitBoard generate_obstacle_map(int current_obstacle_combination, int number_of_bit, BitBoard future_moves);
        BitBoard generate_bishop_attacks_with_obstacles(int sq, BitBoard obstacle_map);
        BitBoard generate_rook_attacks_with_obstacles(int sq, BitBoard obstacle_map);
        Board* board;
        BitBoard knight_movement[64];
        BitBoard king_movement[64];
        BitBoard white_pawn_left_movement[64];
        BitBoard white_pawn_right_movement[64];
        BitBoard black_pawn_left_movement[64];
        BitBoard black_pawn_right_movement[64];
        BitBoard bishop_movement[64];
        BitBoard rook_movement[64];
        int bishop_number_of_reachable_squares[64];
        int rook_number_of_reachable_squares[64];
        BitBoard bishop_magic_square_values[64];
        BitBoard rook_magic_square_values[64];
        BitBoard bishop_attack_masks[64][512];
        BitBoard rook_attack_masks[64][4096];
}
#endif