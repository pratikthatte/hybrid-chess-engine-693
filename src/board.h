#ifndef BOARD_H
#define BOARD_H
#include "typedefs.h"
#include <unordered_map>

class EvaluationEngine;
class MoveGenerationEngine;

class Board{
    public:
        explicit Board(MoveGenerationEngine& moveGenerationEngine, EvaluationEngine& evaluationEngine);
        ~Board();
        BitBoard generate_random_bitboard();
        void init_zobrist_hashing();
        void parsePosition(char* command);
        void getBestMove();
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
    private:
        MoveGenerationEngine& moveEngine;
        EvaluationEngine& evaluationEngine;
        BitBoard board_occupancy;
        BitBoard board_hash;
        BitBoard board_attack_mask;
        int turn;
        int castling;
        int enPassantSq;
        int halfMoveCount;
        int fullMoveCount;
        int whiteKingSq;
        int blackKingSq;
        char* fen_string;
        BitBoard board_squares[64];
        BitBoard zobrist_pieces_hash[12][64];
        BitBoard zobrist_en_passant[64];
        BitBoard zobrist_castling[16];
        BitBoard zobrist_white_to_move;
        void evaluateFen(char* fen);
        int charToPiece(char c);
        BitBoard* getPieceBitBoard(int piece);
        void resetBoard();
        void generateOccupancyMask();
        void generateBoardHash();
        void computeHashAgainstPiece(BitBoard* hash, BitBoard piece, int& index);
        void generateAttackMasks();
        void makeMove(char* moves);
        void populateMove(char* moves, Move* move);
        void implementMove(Move* temp_move);
        void makeEnPassantMove(Move* move);
        void makeCastleMove(Move* move);
        std::unordered_map<Pieces,int> piece_value_map;
        void init_piece_value_map();
        void compareTempBBWithRookAndCastleChanges(BitBoard bb, BitBoard rook, Move* move);
};
#endif