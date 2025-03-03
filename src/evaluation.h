#ifndef EVALUATION_H
#define EVALUATION_H
#include<unordered_map>

class EvaluationEngine{
    public:
        //Constructor
        EvaluationEngine();
        //Destructor
        ~EvaluationEngine();
        //Basic evaluate function assigns score based on number of W and B pieces
        int basic_evaluate(Board* board);
    private:
        uordered_map<Pieces,int> piece_value_map;
        int calculate_number_of_pieces(BitBoard bb);
};
#endif