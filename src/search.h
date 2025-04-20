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
    void populateBestMoveMCTSSearch(Board* board);
    void populateBestMoveMCTS_IR_M(Board* board);
    void populateBestMoveMCTS_IC_M(Board* board);
    void populateBestMoveMCTS_IP_M(Board* board);
private:
    struct TreeNode {
        Board board;    
        Move move;
        TreeNode* parent;
        std::vector<TreeNode*> children;
        std::vector<Move> untriedMoves;
        int visits;
        double totalValue;
        TreeNode(Board b, TreeNode* parent = nullptr)
            : board(b), parent(parent), visits(0), totalValue(0) { }
        ~TreeNode() {
            for (TreeNode* child : children) {
                delete child;
            }
        }
    };
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);
    std::vector<Move> generateLegalMoves(Board &board);
    int evaluateBoard(Board &board);
    Move addMove(Board &board, int fromSq, int toSq, Pieces piece);
    double uctValue(TreeNode* child, int parentVisits, double C);
    TreeNode* selectChild(TreeNode* node);
    int rollout(Board simulationBoard);
    void backpropagate(TreeNode* node, double result);
    double normalizeEvaluation(int eval, int minEval, int maxEval);
    Move selectMoveByMinimax(Board &board, int depth);
    double rollout_IR_M(Board board);
    double rollout_IC_M(Board board, int rolloutMoves);
    void initializeNodeWithMinimax(TreeNode* node, int depth, int gamma);
    TreeNode* expandChildWithMinimax(TreeNode* parent, Move move);
    void printBestMoveFromRoot(TreeNode* root);
};

#endif
