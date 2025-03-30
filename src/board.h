#ifndef BOARD_H
#define BOARD_H

class Board{
    public:
        //Constructor
        Board();
        //Destructor
        ~Board();
        BitBoard generate_random_bitboard();
        void init_zobrist_hashing();
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
        //Array for zobrist hashes of each piece on each square
        BitBoard zobrist_pieces_hash[12][64];
        //Array for zobrist hashes for each square of board in case of en passant
        BitBoard zobrist_en_passant[64];
        // Zobrist hashes for castling, 2 on each side
        BitBoard zobrist_castling[16];
        //Bitboard to represent zobrist hash for white's turn to play. Could be a bool but works better as a Bitboard for XOR purposes.
        BitBoard zobrist_white_to_move;
};
#endif