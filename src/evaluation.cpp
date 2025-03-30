#include<'evaluation.h'>

EvaluationEngine::EvaluationEngine(){
    this->piece_value_map[PAWN_W] = 1;
    this->piece_value_map[PAWN_B] = -1;
    this->piece_value_map[KNIGHT_W] = 3;
    this->piece_value_map[KNIGHT_B] = -3;
    this->piece_value_map[BISHOP_W] = 3;
    this->piece_value_map[BISHOP_B] = -3;
    this->piece_value_map[ROOK_W] = 5;
    this->piece_value_map[ROOK_B] = -5;
    this->piece_value_map[QUEEN_W] = 9;
    this->piece_value_map[QUEEN_B] = -9;
    this->white_pawn_position_values = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10,-20,-20, 10, 10,  5,
        5, -5,-10,  0,  0,-10, -5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0,  0,  0,  0,  0,  0,  0,  0
    };
    this->white_knight_position_values = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };
    this->white_bishop_position_values = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
    this->white_rook_position_values = {
        0,  0,  5,  10, 10, 5,  0,  0,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
        5,  10, 10, 10, 10, 10, 10, 5,
        0,  0,  0,  0,  0,  0,  0,  0,
   };
   this->white_queen_position_values = {
        -20,-10,-10, -5, -5,-10,-10,-20
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
        0,  0,  5,  5,  5,  5,  0, -5,
        -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20,
    };
    this->white_king_position_values = {
        20,  30,  10,  0,   0,   10,  30,  20,
        20,  20,  0,   0,   0,   0,   20,  20,
       -10, -20, -20, -20, -20, -20, -20, -10,
       -20, -30, -30, -40, -40, -30, -30, -20,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
   };
   for(int i=0;i<64;i++){
    this->black_king_position_values[i] = this->white_king_position_values[63-i];
    this->black_queen_position_values[i] = this->white_queen_position_values[63-i];
    this->black_rook_position_values[i] = this->white_rook_position_values[63-i];
    this->black_bishop_position_values[i] = this->white_bishop_position_values[63-i];
    this->black_knight_position_values[i] = this->white_knight_position_values[63-i];
    this->black_pawn_position_values[i] = this->white_pawn_position_values[63-i];
   }
}

EvaluationEngine::~EvaluationEngine(){}
int EvaluationEngine::basic_evaluate(Board* board){
    int result = 0;
    result += calculate_number_of_pieces(board->black_pawn)*this->piece_value_map[PAWN_B];
    result += calculate_number_of_pieces(board->white_pawn)*this->piece_value_map[PAWN_W];
    result += calculate_number_of_pieces(board->black_bishop)*this->piece_value_map[BISHOP_B];
    result += calculate_number_of_pieces(board->white_bishop)*this->piece_value_map[BISHOP_W];
    result += calculate_number_of_pieces(board->black_knight)*this->piece_value_map[KNIGHT_B];
    result += calculate_number_of_pieces(board->white_knight)*this->piece_value_map[KNIGHT_W];
    result += calculate_number_of_pieces(board->black_rook)*this->piece_value_map[ROOK_B];
    result += calculate_number_of_pieces(board->white_rook)*this->piece_value_map[ROOK_W];
    result += calculate_number_of_pieces(board->black_queen)*this->piece_value_map[QUEEN_B];
    result += calculate_number_of_pieces(board->white_queen)*this->piece_value_map[QUEEN_W];
    return result;
}

int EvaluationEngine::calculate_number_of_pieces(Bitboard bb){
    Bitboard tempBB = bb;
    int count = 0;
    while(tempBB!=0){
        tempBB &= (tempBB-1);
        count += 1;
    }
    return count;
}