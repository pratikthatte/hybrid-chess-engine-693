#include "board.h"

Board::Board(){
    for(int i=0;i<64;i++){
        BitBoard tempBB = 1ULL << i;
        this->board_squares[i] = tempBB;
    }
}

Board::~Board(){}

Bitboard Board::generate_random_bitboard(){
    BitBoard temp_bb = 0;
    for(int i=0;i<64;i++){
        temp_bb |= ((BitBoard) rand() % 2) << i;
    }
    return temp_bb;
}

void Board::init_zobrist_hashing(){
    for(int i=0;i<12;i++){
        for(int j=0;j<64;j++){
            this->zobrist_pieces_hash[i][j] = generate_random_bitboard();
        }
    }
    for(int i=0;i<64;i++){
        this->zobrist_en_passant[i] = generate_random_bitboard();
    }
    for(int i=0;i<16;i++){
        this->zobrist_castling[i] = generate_random_bitboard();
    }
    this->zobrist_white_to_move = generate_random_bitboard();
}