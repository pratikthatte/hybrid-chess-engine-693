#include "board.h"

Board::Board(){
    for(int i=0;i<64;i++){
        BitBoard tempBB = 1ULL << i;
        this->board_squares[i] = tempBB;
    }
}

Board::~Board(){
}