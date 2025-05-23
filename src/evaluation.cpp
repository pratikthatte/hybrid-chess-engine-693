#include "evaluation.h"
#include "typedefs.h"
#include "board.h"
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
int EvaluationEngine::basic_evaluate(
    BitBoard white_pawn, BitBoard black_pawn,
    BitBoard white_knight, BitBoard black_knight,
    BitBoard white_bishop, BitBoard black_bishop,
    BitBoard white_rook, BitBoard black_rook,
    BitBoard white_queen, BitBoard black_queen
){
    int result = 0;
    result += calculate_number_of_pieces(black_pawn)*this->piece_value_map[PAWN_B];
    result += calculate_piece_position_values(black_pawn,this->black_pawn_position_values);
    result += calculate_number_of_pieces(white_pawn)*this->piece_value_map[PAWN_W];
    result += calculate_piece_position_values(white_pawn,this->white_pawn_position_values);
    result += calculate_number_of_pieces(black_bishop)*this->piece_value_map[BISHOP_B];
    result += calculate_piece_position_values(black_bishop,this->black_bishop_position_values);
    result += calculate_number_of_pieces(white_bishop)*this->piece_value_map[BISHOP_W];
    result += calculate_piece_position_values(white_bishop,this->white_bishop_position_values);
    result += calculate_number_of_pieces(black_knight)*this->piece_value_map[KNIGHT_B];
    result += calculate_piece_position_values(black_knight,this->black_knight_position_values);
    result += calculate_number_of_pieces(white_knight)*this->piece_value_map[KNIGHT_W];
    result += calculate_piece_position_values(white_knight,this->white_knight_position_values);
    result += calculate_number_of_pieces(black_rook)*this->piece_value_map[ROOK_B];
    result += calculate_piece_position_values(black_rook,this->black_rook_position_values);
    result += calculate_number_of_pieces(white_rook)*this->piece_value_map[ROOK_W];
    result += calculate_piece_position_values(white_rook,this->white_rook_position_values);
    result += calculate_number_of_pieces(black_queen)*this->piece_value_map[QUEEN_B];
    result += calculate_piece_position_values(black_queen,this->black_queen_position_values);
    result += calculate_number_of_pieces(white_queen)*this->piece_value_map[QUEEN_W];
    result += calculate_piece_position_values(white_queen,this->white_queen_position_values);
    return result;
}

int EvaluationEngine::calculate_number_of_pieces(BitBoard bb){
    BitBoard tempBB = bb;
    int count = 0;
    while(tempBB!=0){
        tempBB &= (tempBB-1);
        count += 1;
    }
    return count;
}
int EvaluationEngine::calculate_piece_position_values(BitBoard bb, std::array<int,64>& position_values){
    int score = 0;
    while (bb) {
        int index = __builtin_ctzll(bb);
        score += position_values[index];
        bb &= (bb - 1);
    }
    return score;
}
int EvaluationEngine::evaluate_position_with_king_safety_and_development(Board& board) {
    int eval = this->basic_evaluate(
        board.white_pawn, board.black_pawn,
        board.white_knight, board.black_knight,
        board.white_bishop, board.black_bishop,
        board.white_rook, board.black_rook,
        board.white_queen, board.black_queen
    );

    int whiteKingSq = board.whiteKingSq;
    int blackKingSq = board.blackKingSq;
    BitBoard attackMask = board.board_attack_mask;

    if (attackMask & (1ULL << whiteKingSq)) eval -= 50;
    if (attackMask & (1ULL << blackKingSq)) eval += 50;

    int whiteDeveloped = 0, blackDeveloped = 0;
    BitBoard wPieces = board.white_knight | board.white_bishop | board.white_rook | board.white_queen;
    BitBoard bPieces = board.black_knight | board.black_bishop | board.black_rook | board.black_queen;

    while (wPieces) {
        int sq = __builtin_ctzll(wPieces);
        if (sq / 8 < 7) whiteDeveloped++; 
        wPieces &= wPieces - 1;
    }

    while (bPieces) {
        int sq = __builtin_ctzll(bPieces);
        if (sq / 8 > 0) blackDeveloped++;
        bPieces &= bPieces - 1;
    }

    eval += (whiteDeveloped - blackDeveloped) * 5;
    return eval;
}