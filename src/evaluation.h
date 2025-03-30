#ifndef EVALUATION_H
#define EVALUATION_H
#include<unordered_map>

class EvaluationEngine{
    public:
        //Constructor
        EvaluationEngine();
        //Destructor
        ~EvaluationEngine();
        //Basic evaluate function assigns score based on number of W and B pieces
        int basic_evaluate(Board* board);
    private:
        uordered_map<Pieces,int> piece_value_map;
        int calculate_number_of_pieces(BitBoard bb);
        int white_pawn_position_values[64];
        int black_pawn_position_values[64];
        int white_knight_position_values[64];
        int black_knight_position_values[64];
        int white_rook_position_values[64];
        int black_rook_position_values[64];
        int white_bishop_position_values[64];
        int black_bishop_position_values[64];
        int white_queen_position_values[64];
        int black_queen_position_values[64];
        int white_king_position_values[64];
        int black_king_position_values[64];
};
#endif