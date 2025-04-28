#ifndef SEARCH_H
#define SEARCH_H
#include <vector>
#include <chrono>
#include <limits>
#include <unordered_map>
#include "board.h"
#include "evaluation.h"
#include "typedefs.h"
enum NodeType { EXACT = 0, LOWERBOUND = 1, UPPERBOUND = 2 };

class SearchEngine {
public:
    explicit SearchEngine(EvaluationEngine& engine);
    void setTimeLimit(int ms);
    void populateBestMoveMinimaxSearch(Board* board);
    std::vector<Move> generateLegalMoves(Board& board);
    std::vector<Move> filterLegalMoves(Board& board, const std::vector<Move>& moves);

private:
    static constexpr int MAX_PLY = 64;

    EvaluationEngine& evalEngine;
    int timeLimitMs;
    int searchDepth;
    std::unordered_map<BitBoard, TTEntry> ttTable;
    Move killers[MAX_PLY][2];
    int historyScores[64][64];

    Move addMove(const Board& board, int from, int to, Pieces pieceType);
    int negamax(Board& board, int depth, int alpha, int beta, Move& best, int ply=0);
    std::string moveToUCI(const Move& m);
    static int pieceValue(Pieces p);
};

#endif
