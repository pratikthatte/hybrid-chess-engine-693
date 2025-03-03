#ifndef BOARD_H
#define BOARD_H

class Board{
    public:
        //Constructor
        Board();
        //Destructor
        ~Board();
    private:
        //Declaring BitBoards for 12 distinct piece types 
        BitBoard black_pawn;
        BitBoard white_pawn;
        BitBoard black_knight;
        BitBoard white_knight;
        BitBoard black_rook;
        BitBoard white_rook;
        BitBoard black_bishop;
        BitBoard white_bishop;
        BitBoard black_king;
        BitBoard white_king;
        BitBoard black_queen;
        BitBoard white_queen;
        // Array to store 64 unsigned long longs each left shifted by 1 from the previous value. Board square representation
        BitBoard board_squares[64];
};
#endif