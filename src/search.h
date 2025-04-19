#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include <vector>

static const int SEARCH_DEPTH = 7;
class SearchEngine {
public:
    SearchEngine();
    ~SearchEngine();
    void populateBestMoveMinimaxSearch(Board* board);
private:
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);
    std::vector<Move> generateLegalMoves(Board &board);
    int evaluateBoard(Board &board);
    Move addMove(Board &board, int fromSq, int toSq, Pieces piece);
};

#endif
