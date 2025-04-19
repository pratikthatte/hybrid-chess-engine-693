#include <cstdio>
#include <cstring>
#include <memory>
#include "board.h"
#include "move.h"
#include "search.h"
#include "evaluation.h"

int main(){
    std::unique_ptr<MoveGenerationEngine> moveEngine = std::make_unique<MoveGenerationEngine>();
    std::unique_ptr<EvaluationEngine> evaluationEngine = std::make_unique<EvaluationEngine>();
    Board* board = new Board(*moveEngine, *evaluationEngine);
    SearchEngine* searchEngine = new SearchEngine();
    std::setbuf(stdin,NULL);
    std::setbuf(stdout,NULL);
    int buffersize = 2000;
    char input[buffersize];
    while(true){
        memset(input,'\0',sizeof(input));
        fflush(stdout);
        if(!fgets(input,buffersize,stdin)){
            break;
        }
        if(input[0]=='\n'){
            continue;
        }
        if(strncmp(input,"uci",3)==0){
            std::printf("");
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
            searchEngine->populateBestMoveMinimaxSearch(board);
        }

    }
    return 0;
}