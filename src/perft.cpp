#include <iostream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>
#include <sstream>

#include "board.h"
#include "move.h"
#include "search.h"
#include "evaluation.h"

// Helper function: convert a Move to UCI-style string
std::string moveToString(const Move &move) {
    char buf[6];
    int fromCol = move.fromSquare % 8;
    int fromRow = move.fromSquare / 8;
    int toCol = move.toSquare % 8;
    int toRow = move.toSquare / 8;
    buf[0] = 'a' + fromCol;
    buf[1] = '8' - fromRow;
    buf[2] = 'a' + toCol;
    buf[3] = '8' - toRow;
    // If the move is a promotion move (i.e. promotion field != pieceType)
    if (move.promotion != move.pieceType) {
        switch (move.promotion) {
            case QUEEN_W:
            case QUEEN_B:
                buf[4] = 'q';
                break;
            case ROOK_W:
            case ROOK_B:
                buf[4] = 'r';
                break;
            case KNIGHT_W:
            case KNIGHT_B:
                buf[4] = 'n';
                break;
            case BISHOP_W:
            case BISHOP_B:
                buf[4] = 'b';
                break;
            default:
                buf[4] = '\0';
                break;
        }
        buf[5] = '\0';
    } else {
        buf[4] = '\0';
    }
    return std::string(buf);
}

// Recursive perft function that returns the number of leaf nodes reached
uint64_t perft(Board &board, int depth, SearchEngine &se) {
    std::cout << "Entering perft at depth " << depth << std::endl;
    
    if (depth == 0)
        return 1ULL;
    
    std::vector<Move> moves = se.generateLegalMoves(board);
    
    if (depth == 1) {
        std::cerr << "Depth 1: " << moves.size() << " moves" << std::endl;
        return moves.size();
    }
    
    uint64_t nodes = 0;
    for (const Move &move : moves) {
        Board nextBoard = board;
        nextBoard.implementMove(const_cast<Move*>(&move));
        uint64_t subtreeNodes = perft(nextBoard, depth - 1, se);
        nodes += subtreeNodes;
    }
    std::cerr << "Exiting perft at depth " << depth << " with nodes = " << nodes << std::endl;
    return nodes;
}

int main(int argc, char **argv) {
    std::cout << "Inside main " << std::endl;
    std::cout.flush();
    // Set the depth; default is 4 if not provided
    int depth = 4;
    if (argc > 1) {
        depth = std::atoi(argv[1]);
        if (depth < 1) {
            std::cerr << "Depth must be at least 1" << std::endl;
            return 1;
        }
    }
    
    // Use smart pointers to create the engine components
    auto moveEngine = std::make_unique<MoveGenerationEngine>();
    auto evaluationEngine = std::make_unique<EvaluationEngine>();
    
    // Initialize the board to the starting position
    Board board(*moveEngine, *evaluationEngine);
    std::cout<<"Entering parsePosition"<<std::endl;
    board.parsePosition("position startpos");
    
    // Create a search engine instance (using automatic storage is fine here)
    SearchEngine se;
    
    // Start timing
    auto startTime = std::chrono::steady_clock::now();

    // Generate legal moves from the root position and log each branch's count:
    std::vector<Move> rootMoves = se.generateLegalMoves(board);
    uint64_t totalNodes = 0;
    
    std::cout << "Perft results for depth " << depth << " from starting position:" << std::endl;
    
    // For each move from the root, get the number of child nodes at depth-1
    for (const Move &move : rootMoves) {
        Board branchBoard = board;
        branchBoard.implementMove(const_cast<Move*>(&move));
        uint64_t branchNodes = perft(branchBoard, depth - 1, se);
        std::string moveStr = moveToString(move);
        std::cout << moveStr << ": " << branchNodes << " nodes" << std::endl;
        totalNodes += branchNodes;
    }
    
    // End timing
    auto endTime = std::chrono::steady_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();
    
    // Print the total nodes and timing information
    std::cout << "Total nodes: " << totalNodes << " (in " << elapsedSeconds << " seconds)" << std::endl;
    
    return 0;
}
