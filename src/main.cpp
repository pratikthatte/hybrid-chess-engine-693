#include "board.h"

int main(){
    Board* board = new Board();
    board->init_zobrist_hashing();
    
    return 0;
}