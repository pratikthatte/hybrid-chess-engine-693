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
}

EvaluationEngine::~EvaluationEngine(){}
EvaluationEngine::basic_evaluate(Board* board){
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

EvaluationEngine::calculate_number_of_pieces(Bitboard bb){
    Bitboard tempBB = bb;
    int count = 0;
    while(tempBB!=0){
        tempBB &= (tempBB-1);
        count += 1;
    }
    return count;
}