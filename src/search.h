#ifndef SEARCH_H
#define SEARCH_H
#include <vector>
#include <chrono>
#include <limits>
#include <unordered_map>
#include <random>
#include "board.h"
#include "evaluation.h"
#include "typedefs.h"
enum NodeType { EXACT = 0, LOWERBOUND = 1, UPPERBOUND = 2 };

class SearchEngine {
public:
    explicit SearchEngine(EvaluationEngine& engine);
    void setTimeLimit(int ms);
    void populateBestMoveMinimaxSearch(Board* board);
    void populateBestMoveMCTSSearch(Board* board);
    void populateBestMoveMCTS_IR_M(Board* board);
    void populateBestMoveMCTS_IC_M(Board* board);
    void populateBestMoveMCTS_IP_M(Board* board);
    std::vector<Move> generateLegalMoves(Board& board);
    std::vector<Move> filterLegalMoves(Board& board, const std::vector<Move>& moves);

private:
    static constexpr int MAX_PLY = 64;
    struct MCTSNode {
        MCTSNode* parent;
        Move move;
        std::vector<MCTSNode*> children;
        std::vector<Move> untriedMoves;
        int visits;
        double wins;
        int playerToMove;
        MCTSNode(MCTSNode* p, const Move& m, int player)
          : parent(p), move(m), visits(0), wins(0.0), playerToMove(player) {}
    };
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
    MCTSNode* selectNode(MCTSNode* node);
    MCTSNode* expandNode(MCTSNode* node, Board& state);
    double simulatePlayout(Board state, int maxPlies, int rootPlayer);
    void backpropagate(MCTSNode* node, double result);
    std::mt19937 mctsRng;
    double explorationConstant;
};

#endif