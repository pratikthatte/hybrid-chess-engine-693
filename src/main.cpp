#include <cstdio>
#include <iostream>
#include <cstring>
#include <memory>
#include "board.h"
#include "move.h"
#include "search.h"
#include "evaluation.h"
#include <fstream>
#include <sstream>

std::unordered_map<std::string, std::vector<std::string>> openingBook;

void loadOpeningBook(const std::string& filename) {
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string fen;
        std::getline(iss, fen, ' ');
        for (int i = 0; i < 5; ++i) {
            std::string part;
            iss >> part;
            fen += " " + part;
        }

        std::string move;
        while (iss >> move) {
            openingBook[fen].push_back(move);
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);  
    std::unique_ptr<MoveGenerationEngine> moveEngine = std::make_unique<MoveGenerationEngine>();
    std::unique_ptr<EvaluationEngine> evaluationEngine = std::make_unique<EvaluationEngine>();
    std::unique_ptr<Board> board = std::make_unique<Board>(*moveEngine, *evaluationEngine);
    std::unique_ptr<SearchEngine> searchEngine = std::make_unique<SearchEngine>(*evaluationEngine);
    //loadOpeningBook("book.txt");
    std::setbuf(stdin,NULL);
    std::setbuf(stdout,NULL);
    int buffersize = 2000;
    char input[buffersize];
    while(true){
        memset(input,'\0',sizeof(input));
        fflush(stdout);
        if (!fgets(input, buffersize, stdin)) {
            break;
        }
        printf("Received input: [%s]\n", input);
        printf("Input is: %s\n", input);
        fflush(stdout);
        if(input[0]=='\n'){
            continue;
        }
        if (strncmp(input, "isready", 7) == 0) {
            std::printf("readyok\n");
            continue;
        }
        else if (strncmp(input, "position", 8) == 0){
            board->parsePosition(input);
        }
        else if (strncmp(input, "ucinewgame", 10) == 0){
            board->parsePosition("position startpos");
        }
        else if (strncmp(input, "go", 2) == 0){
            int movetime = -1;
            std::string fen = board->getFEN();
            if (openingBook.count(fen)) {
                const auto& bookMoves = openingBook[fen];
                std::vector<Move> pseudoLegalMoves = searchEngine->generateLegalMoves(*board);
                std::vector<Move> legalMoves = searchEngine->filterLegalMoves(*board,pseudoLegalMoves);

                for (const auto& bookMove : bookMoves) {
                    for (const auto& m : legalMoves) {
                        char moveStr[6];
                        int fromCol = m.fromSquare % 8;
                        int fromRow = m.fromSquare / 8;
                        int toCol = m.toSquare % 8;
                        int toRow = m.toSquare / 8;
                        moveStr[0] = 'a' + fromCol;
                        moveStr[1] = '8' - fromRow;
                        moveStr[2] = 'a' + toCol;
                        moveStr[3] = '8' - toRow;
                        if ((m.promotion == QUEEN_W || m.promotion == QUEEN_B) &&
                            (m.toSquare < 8 || m.toSquare >= 56)) {
                            moveStr[4] = 'q';
                            moveStr[5] = '\0';
                        } else {
                            moveStr[4] = '\0';
                        }

                        if (bookMove == moveStr) {
                            std::cout << "bestmove " << bookMove << std::endl;
                            fflush(stdout);
                            goto skip_search;
                        }
                    }
                }
            }
            // Parse movetime (in milliseconds) from the input.
         
            if (strstr(input, "movetime") != NULL) {
                sscanf(input, "go movetime %d", &movetime);
            }
            // If not specified, we default to 300000 ms (5 minutes).
            if (movetime == -1)
                movetime = 300000;
            
            // Pass the time limit to the search engine.
            searchEngine->setTimeLimit(movetime);
        
            // Run the chosen search. For example, if using MCTS_IP:
            #ifdef USE_MCTS_IP
                searchEngine->populateBestMoveMCTS_IP_M(board.get());
            #elif defined(USE_MINIMAX)
                searchEngine->populateBestMoveMinimaxSearch(board.get());
            #elif defined(USE_MCTS)
                searchEngine->populateBestMoveMCTSSearch(board.get());
            #elif defined(USE_MCTS_IR)
                searchEngine->populateBestMoveMCTS_IR_M(board.get());
            #elif defined(USE_MCTS_IC)
                searchEngine->populateBestMoveMCTS_IC_M(board.get());
            #endif
            skip_search: ;
            // (If using an opening book, that code remains as before.)
        }
        else if (strncmp(input,"quit",4) == 0){
            break;
        }
        else if(strncmp(input,"uci",3) == 0){
            printf("id name HybridEngine\n");
            printf("id author Pratik\n");
            printf("uciok\n");
        }

    }
    return 0;
}