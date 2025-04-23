#ifndef SEARCH_OPTIMIZED_H
#define SEARCH_OPTIMIZED_H

#include "board.h"
#include "move.h"
#include <vector>
#include <unordered_map>
#include <chrono>

// Use a lower search depth for faster minimax in tournaments.
static const int SEARCH_DEPTH = 3;

class SearchEngine {
public:
    // Constructor takes a reference to the evaluation engine.
    explicit SearchEngine(EvaluationEngine& evaluationEngine);
    ~SearchEngine();

    // Set a per-move time limit for MCTS in milliseconds.
    void setTimeLimit(int ms) { timeLimitMs = ms; }

    // Main search interfaces for different algorithms.
    void populateBestMoveMinimaxSearch(Board* board);
    void populateBestMoveMCTSSearch(Board* board);
    void populateBestMoveMCTS_IR_M(Board* board);
    void populateBestMoveMCTS_IC_M(Board* board);
    void populateBestMoveMCTS_IP_M(Board* board);

    // (Optional) iterative deepening for minimax.
    Move iterativeDeepening(Board* board, int maxTimeMs);

    // Generate pseudo-legal moves and filter out illegal moves.
    std::vector<Move> generateLegalMoves(Board &board);
    std::vector<Move> filterLegalMoves(const Board &board, const std::vector<Move> &pseudoLegalMoves);

private:
    // Time limit in ms.
    int timeLimitMs = 300000; // default 5 minutes
    EvaluationEngine& evaluationEngine;
    // Transposition table for minimax.
    std::unordered_map<BitBoard, TTEntry> transpositionTable;

    // Tree node structure for MCTS.
    struct TreeNode {
        Board board;    
        Move move;
        TreeNode* parent;
        std::vector<TreeNode*> children;
        std::vector<Move> untriedMoves;
        int visits;
        double totalValue;
        TreeNode(Board b, TreeNode* parent = nullptr)
            : board(b), parent(parent), visits(0), totalValue(0.0) { }
        ~TreeNode() {
            for (TreeNode* child : children) {
                delete child;
            }
        }
    };

    // Minimax search with alpha-beta pruning.
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);

    // Evaluation function call.
    int evaluateBoard(Board &board);

    // Helper: add a move.
    Move addMove(Board &board, int fromSq, int toSq, Pieces piece);

    // UCT value computation for MCTS.
    double uctValue(TreeNode* child, int parentVisits, double C);
    // Select a child using UCT.
    TreeNode* selectChild(TreeNode* node);

    // Rollout simulation.
    int rollout(Board simulationBoard);
    // Backpropagation function.
    void backpropagate(TreeNode* node, double result);

    // Normalize an evaluation value to [0,1].
    double normalizeEvaluation(int eval, int minEval, int maxEval);
    // Select a move via minimax (used e.g. in MCTS-IR rollouts).
    Move selectMoveByMinimax(Board &board, int depth);

    // Functions to integrate heuristic evaluations into MCTS node priors.
    void initializeNodeWithMinimax(TreeNode* node, int depth, int gamma);
    TreeNode* expandChildWithMinimax(TreeNode* parent, Move move);

    // Print best move from root node.
    void printBestMoveFromRoot(TreeNode* root);

    // Heuristic move scoring and ordering.
    int scoreMove(const Move& move);
    void orderMoves(std::vector<Move>& moves);
};

#endif
