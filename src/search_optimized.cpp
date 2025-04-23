#include "search.h"  // or "search_optimized.h" if you prefer
#include "evaluation.h"
#include "move.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>

// Using namespace std for brevity in this sample.
using namespace std;
using namespace std::chrono;

// Node type values for the transposition table.
enum NodeType {
    EXACT = 0,
    LOWERBOUND = 1,
    UPPERBOUND = 2
};

SearchEngine::SearchEngine(EvaluationEngine& evaluationEngine)
    : evaluationEngine(evaluationEngine) { }

SearchEngine::~SearchEngine() { }

int SearchEngine::evaluateBoard(Board &board) {
    return board.basic_evaluate();
}

int SearchEngine::scoreMove(const Move& move) {
    int score = 0;
    // Favor promotions
    if (move.promotion == QUEEN_W || move.promotion == QUEEN_B)
        score += 1000;

    int to = move.toSquare;

    switch (move.pieceType) {
        case PAWN_W:   score += 10 + evaluationEngine.white_pawn_position_values[to]; break;
        case KNIGHT_W: score += 30 + evaluationEngine.white_knight_position_values[to]; break;
        case BISHOP_W: score += 35 + evaluationEngine.white_bishop_position_values[to]; break;
        case ROOK_W:   score += 50 + evaluationEngine.white_rook_position_values[to]; break;
        case QUEEN_W:  score += 90 + evaluationEngine.white_queen_position_values[to]; break;
        case KING_W:   score += evaluationEngine.white_king_position_values[to]; break;
        case PAWN_B:   score += 10 + evaluationEngine.black_pawn_position_values[to]; break;
        case KNIGHT_B: score += 30 + evaluationEngine.black_knight_position_values[to]; break;
        case BISHOP_B: score += 35 + evaluationEngine.black_bishop_position_values[to]; break;
        case ROOK_B:   score += 50 + evaluationEngine.black_rook_position_values[to]; break;
        case QUEEN_B:  score += 90 + evaluationEngine.black_queen_position_values[to]; break;
        case KING_B:   score += evaluationEngine.black_king_position_values[to]; break;
        default: break;
    }
    return score;
}

void SearchEngine::orderMoves(vector<Move>& moves) {
    // Shuffle to avoid deterministic tie-breaks.
    std::shuffle(moves.begin(), moves.end(), mt19937(random_device{}()));
    sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return scoreMove(a) > scoreMove(b);
    });
}

vector<Move> SearchEngine::generateLegalMoves(Board &board) {
    // Call the move generator (which now produces pseudo‑legal moves with extra features)
    vector<Move> pseudoLegalMoves = board.moveEngine.generateLegalMoves(board);
    // Then filter out moves that leave the king in check:
    return filterLegalMoves(board, pseudoLegalMoves);
}

vector<Move> SearchEngine::filterLegalMoves(const Board &board, const vector<Move> &pseudoLegalMoves) {
    vector<Move> legalMoves;
    // For each pseudo‑legal move, simulate it and update the attack mask.
    for (const Move &move : pseudoLegalMoves) {
        Board boardCopy = board;
        boardCopy.implementMove(const_cast<Move*>(&move));
        boardCopy.generateAttackMasks();
        if (board.turn == 1) {
            // For White, if the white king is not attacked, the move is legal.
            if ((boardCopy.board_attack_mask & (1ULL << boardCopy.whiteKingSq)) == 0)
                legalMoves.push_back(move);
        } else {
            if ((boardCopy.board_attack_mask & (1ULL << boardCopy.blackKingSq)) == 0)
                legalMoves.push_back(move);
        }
    }
    return legalMoves;
}

int SearchEngine::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    BitBoard hash = board.board_hash;
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end()) {
        const TTEntry& entry = it->second;
        if (entry.depth >= depth) {
            if (entry.nodeType == EXACT) return entry.eval;
            if (entry.nodeType == LOWERBOUND && entry.eval >= beta) return entry.eval;
            if (entry.nodeType == UPPERBOUND && entry.eval <= alpha) return entry.eval;
        }
    }

    vector<Move> pseudoLegalMoves = generateLegalMoves(board);
    if (depth == 0 || pseudoLegalMoves.empty())
        return evaluateBoard(board);

    int originalAlpha = alpha;
    int bestEval;

    if (maximizingPlayer) {
        bestEval = INT_MIN;
        orderMoves(pseudoLegalMoves);
        vector<Move> legalMoves = pseudoLegalMoves;  // Already filtered in generateLegalMoves.
        for (const Move &move : legalMoves) {
            Board copy = board;
            copy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(copy, depth - 1, alpha, beta, false);
            bestEval = max(bestEval, eval);
            alpha = max(alpha, bestEval);
            if (beta <= alpha) break;
        }
    } else {
        bestEval = INT_MAX;
        orderMoves(pseudoLegalMoves);
        vector<Move> legalMoves = pseudoLegalMoves;
        for (const Move &move : legalMoves) {
            Board copy = board;
            copy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(copy, depth - 1, alpha, beta, true);
            bestEval = min(bestEval, eval);
            beta = min(beta, bestEval);
            if (beta <= alpha) break;
        }
    }

    TTEntry entry;
    entry.zobrist = hash;
    entry.eval = bestEval;
    entry.depth = depth;
    entry.nodeType = (bestEval <= originalAlpha) ? UPPERBOUND :
                     (bestEval >= beta) ? LOWERBOUND : EXACT;
    transpositionTable[hash] = entry;

    return bestEval;
}

Move SearchEngine::addMove(Board &board, int fromSq, int toSq, Pieces piece) {
    Move m;
    m.fromSquare = fromSq;
    m.toSquare = toSq;
    m.pieceType = piece;
    m.castle = 0;
    // Always promote to queen, as usual in engine implementations.
    if (piece == PAWN_W && toSq < 8)
        m.promotion = QUEEN_W;
    else if (piece == PAWN_B && toSq >= 56)
        m.promotion = QUEEN_B;
    else
        m.promotion = piece;
    return m;
}

double SearchEngine::uctValue(TreeNode* child, int parentVisits, double C) {
    if (child->visits == 0) return numeric_limits<double>::max();
    return child->totalValue / child->visits + C * sqrt(log(parentVisits) / child->visits);
}

SearchEngine::TreeNode* SearchEngine::selectChild(TreeNode* node) {
    TreeNode* bestChild = nullptr;
    double bestVal = -numeric_limits<double>::infinity();
    for (TreeNode* child : node->children) {
        double val = uctValue(child, node->visits, 1.4);
        if (val > bestVal) {
            bestVal = val;
            bestChild = child;
        }
    }
    return bestChild;
}

int SearchEngine::rollout(Board simulationBoard) {
    const int rolloutDepth = 10;
    for (int i = 0; i < rolloutDepth; i++) {
        vector<Move> pseudoMoves = generateLegalMoves(simulationBoard);
        if (pseudoMoves.empty())
            break;
        int idx = rand() % pseudoMoves.size();
        simulationBoard.implementMove(const_cast<Move*>(&pseudoMoves[idx]));
    }
    return simulationBoard.basic_evaluate();
}

void SearchEngine::backpropagate(TreeNode* node, double result) {
    while (node) {
        node->visits++;
        node->totalValue += result;
        node = node->parent;
    }
}

void SearchEngine::populateBestMoveMinimaxSearch(Board* board) {
    vector<Move> pseudoLegalMoves = generateLegalMoves(*board);
    if (pseudoLegalMoves.empty()) {
        printf("bestmove (none)\n");
        fflush(stdout);
        return;
    }
    int bestValue = INT_MIN;
    Move bestMove = pseudoLegalMoves[0];
    for (const Move &move : pseudoLegalMoves) {
        Board boardCopy = *board;
        boardCopy.implementMove(const_cast<Move*>(&move));
        int eval = minimax(boardCopy, SEARCH_DEPTH - 1, INT_MIN, INT_MAX, false);
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = move;
        }
    }
    // Confirm move is legal.
    vector<Move> confirmMoves = filterLegalMoves(*board, generateLegalMoves(*board));
    bool valid = false;
    for (const Move& m : confirmMoves) {
        if (m.fromSquare == bestMove.fromSquare && m.toSquare == bestMove.toSquare) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        printf("bestmove (none)\n");
        fflush(stdout);
        return;
    }
    char moveStr[6];
    int fromCol = bestMove.fromSquare % 8;
    int fromRow = bestMove.fromSquare / 8;
    int toCol = bestMove.toSquare % 8;
    int toRow = bestMove.toSquare / 8;
    moveStr[0] = 'a' + fromCol;
    moveStr[1] = '8' - fromRow;
    moveStr[2] = 'a' + toCol;
    moveStr[3] = '8' - toRow;
    moveStr[4] = '\0';
    printf("bestmove %s\n", moveStr);
    fflush(stdout);
}

// The following three functions implement our various MCTS hybrids.
// Each function uses the filtered legal moves and uses a time-based limit in the MCTS-IP variant.

void SearchEngine::populateBestMoveMCTSSearch(Board* board) {
    srand((unsigned int)time(nullptr));
    TreeNode* root = new TreeNode(*board);
    vector<Move> pseudoUntriedMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board, pseudoUntriedMoves);
    const int ITERATIONS = 1000;
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            vector<Move> childPseudo = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard, childPseudo);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout(*simulationBoard);
        backpropagate(node, result);
    }
    // Choose the child with the highest visit count.
    TreeNode* bestChild = nullptr;
    int bestVisits = -1;
    for (TreeNode* child : root->children) {
        if (child->visits > bestVisits) {
            bestVisits = child->visits;
            bestChild = child;
        }
    }
    if (bestChild) {
        int fromCol = bestChild->move.fromSquare % 8;
        int fromRow = bestChild->move.fromSquare / 8;
        int toCol = bestChild->move.toSquare % 8;
        int toRow = bestChild->move.toSquare / 8;
        char moveStr[6];
        moveStr[0] = 'a' + fromCol;
        moveStr[1] = '8' - fromRow;
        moveStr[2] = 'a' + toCol;
        moveStr[3] = '8' - toRow;
        moveStr[4] = '\0';
        printf("bestmove %s\n", moveStr);
    } else {
        printf("bestmove (none)\n");
    }
    delete root;
}

void SearchEngine::populateBestMoveMCTS_IR_M(Board* board) {
    srand((unsigned int)time(nullptr));
    TreeNode* root = new TreeNode(*board);
    vector<Move> pseudoUntried = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board, pseudoUntried);
    const int ITERATIONS = 10; // Lower iterations for IR-M due to higher cost per move.
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            vector<Move> childPseudo = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard, childPseudo);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout_IR_M(*simulationBoard);
        backpropagate(node, result);
    }
    printBestMoveFromRoot(root);
    delete root;
}

double SearchEngine::rollout_IR_M(Board board) {
    int rolloutDepth = 3;
    for (int i = 0; i < rolloutDepth; i++) {
        vector<Move> pseudoLegal = generateLegalMoves(board);
        vector<Move> legal = filterLegalMoves(board, pseudoLegal);
        if (legal.empty()) break;
        // Use a shallow minimax search (depth 1) to choose a rollout move.
        Move move = selectMoveByMinimax(board, 1);
        board.implementMove(&move);
    }
    for (int i = 0; i < 10; i++) {
        vector<Move> pseudoLegal = generateLegalMoves(board);
        vector<Move> legal = filterLegalMoves(board, pseudoLegal);
        if (legal.empty()) break;
        orderMoves(legal);
        board.implementMove(&legal[0]);
    }
    return normalizeEvaluation(board.basic_evaluate(), -1000, 1000);
}

double SearchEngine::rollout_IC_M(Board board, int rolloutMoves) {
    for (int i = 0; i < rolloutMoves; i++) {
        vector<Move> pseudoLegal = generateLegalMoves(board);
        vector<Move> legal = filterLegalMoves(board, pseudoLegal);
        if (legal.empty()) break;
        orderMoves(legal);
        board.implementMove(&legal[0]);
    }
    vector<Move> pseudoMoves = generateLegalMoves(board);
    vector<Move> legal = filterLegalMoves(board, pseudoMoves);
    if (legal.empty()) {
        return board.getResult(legal);
    } else {
        int eval = minimax(board, 1, INT_MIN, INT_MAX, true);
        return normalizeEvaluation(eval, -1000, 1000);
    }
}

void SearchEngine::populateBestMoveMCTS_IC_M(Board* board) {
    srand((unsigned int)time(nullptr));
    TreeNode* root = new TreeNode(*board);
    vector<Move> pseudoMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board, pseudoMoves);
    const int ITERATIONS = 10;
    for (int i = 0; i < ITERATIONS; i++) {
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            vector<Move> childPseudo = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard, childPseudo);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout_IC_M(*simulationBoard);
        backpropagate(node, result);
    }
    printBestMoveFromRoot(root);
    delete root;
}

void SearchEngine::populateBestMoveMCTS_IP_M(Board* board) {
    srand((unsigned int)time(nullptr));
    TreeNode* root = new TreeNode(*board);
    vector<Move> pseudoMoves = generateLegalMoves(*board);
    root->untriedMoves = filterLegalMoves(*board, pseudoMoves);
    // Initialize node prior with an embedded minimax search.
    initializeNodeWithMinimax(root, 1, 100);
    
    // Use a time-based limit for the MCTS iterations.
    int allowedMs = (timeLimitMs > 0 ? timeLimitMs : 300000);
    auto start = steady_clock::now();
    int iterations = 0;
    while (true) {
        auto now = steady_clock::now();
        if (duration_cast<milliseconds>(now - start).count() >= allowedMs)
            break;
        TreeNode* node = root;
        Board* simulationBoard = &root->board;
        while (node->untriedMoves.empty() && !node->children.empty()) {
            node = selectChild(node);
            simulationBoard = &node->board;
        }
        if (!node->untriedMoves.empty()) {
            Move m = node->untriedMoves.back();
            node->untriedMoves.pop_back();
            simulationBoard->implementMove(&m);
            TreeNode* child = new TreeNode(*simulationBoard, node);
            child->move = m;
            vector<Move> childPseudo = generateLegalMoves(*simulationBoard);
            child->untriedMoves = filterLegalMoves(*simulationBoard, childPseudo);
            node->children.push_back(child);
            node = child;
        }
        double result = rollout(*simulationBoard);
        backpropagate(node, result);
        iterations++;
    }
    cout << "[DEBUG] MCTS_IP iterations: " << iterations << endl;
    printBestMoveFromRoot(root);
    delete root;
}

double SearchEngine::normalizeEvaluation(int eval, int minEval, int maxEval) {
    return (double)(eval - minEval) / (maxEval - minEval);
}

Move SearchEngine::selectMoveByMinimax(Board &board, int depth) {
    vector<Move> pseudoLegalMoves = generateLegalMoves(board);
    if (pseudoLegalMoves.empty()) return Move{};
    orderMoves(pseudoLegalMoves);
    Move bestMove = pseudoLegalMoves[0];
    int bestEval = INT_MIN;
    for (const Move& move : pseudoLegalMoves) {
        Board copy = board;
        copy.implementMove(const_cast<Move*>(&move));
        int eval = minimax(copy, depth - 1, INT_MIN, INT_MAX, false);
        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}

void SearchEngine::initializeNodeWithMinimax(TreeNode* node, int depth, int gamma) {
    int eval = minimax(node->board, depth, INT_MIN, INT_MAX, true);
    double normEval = normalizeEvaluation(eval, -1000, 1000);
    node->visits += gamma;
    node->totalValue += gamma * normEval;
}

SearchEngine::TreeNode* SearchEngine::expandChildWithMinimax(TreeNode* parent, Move move) {
    Board nextBoard = parent->board;
    nextBoard.implementMove(&move);
    TreeNode* child = new TreeNode(nextBoard, parent);
    child->move = move;
    vector<Move> childPseudo = generateLegalMoves(nextBoard);
    child->untriedMoves = filterLegalMoves(nextBoard, childPseudo);
    initializeNodeWithMinimax(child, 2, 100);
    parent->children.push_back(child);
    return child;
}

void SearchEngine::printBestMoveFromRoot(TreeNode* root) {
    TreeNode* bestChild = nullptr;
    int bestVisits = -1;
    for (TreeNode* child : root->children) {
        if (child->visits > bestVisits) {
            bestVisits = child->visits;
            bestChild = child;
        }
    }
    if (bestChild) {
        vector<Move> pseudoLegal = generateLegalMoves(root->board);
        vector<Move> legal = filterLegalMoves(root->board, pseudoLegal);
        bool valid = false;
        for (const Move& m : legal) {
            if (m.fromSquare == bestChild->move.fromSquare &&
                m.toSquare == bestChild->move.toSquare) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            printf("bestmove (none)\n");
            fflush(stdout);
            return;
        }
        int fromCol = bestChild->move.fromSquare % 8;
        int fromRow = bestChild->move.fromSquare / 8;
        int toCol = bestChild->move.toSquare % 8;
        int toRow = bestChild->move.toSquare / 8;
        char moveStr[6];
        moveStr[0] = 'a' + fromCol;
        moveStr[1] = '8' - fromRow;
        moveStr[2] = 'a' + toCol;
        moveStr[3] = '8' - toRow;
        moveStr[4] = '\0';
        printf("bestmove %s\n", moveStr);
        fflush(stdout);
    } else {
        printf("bestmove (none)\n");
        fflush(stdout);
    }
}

// An optional iterative deepening routine for minimax.
// This function repeatedly deepens the minimax search until the allowed time expires.
Move SearchEngine::iterativeDeepening(Board* board, int maxTimeMs) {
    auto start = steady_clock::now();
    Move bestMove;
    int depth = 1;
    while (true) {
        // Check time elapsed.
        auto now = steady_clock::now();
        if (duration_cast<milliseconds>(now - start).count() >= maxTimeMs)
            break;
        vector<Move> pseudoLegal = generateLegalMoves(*board);
        if (pseudoLegal.empty()) break;
        orderMoves(pseudoLegal);
        int bestEval = INT_MIN;
        Move currentBest = pseudoLegal[0];
        for (const Move& move : pseudoLegal) {
            Board copy = *board;
            copy.implementMove(const_cast<Move*>(&move));
            int eval = minimax(copy, depth, INT_MIN, INT_MAX, false);
            if (eval > bestEval) {
                bestEval = eval;
                currentBest = move;
            }
        }
        bestMove = currentBest;
        depth++;
    }
    return bestMove;
}