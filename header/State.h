//
// Created by berg on 28/01/19.
//

#ifndef BEVA_STATE_H
#define BEVA_STATE_H

#include <unordered_map>
#include "EditVector.h"

using namespace std;

class State {
public:
    State(EditVector* editVector, int id, bool isInitial = false, bool isFinal = false);
    ~State();
    int getEditDistance(int);

    EditVector* editVector;
    unordered_map<unsigned, State*> transitions;
    bool isInitial;
    bool isFinal;
    int id;
    VectorChar display();
//    string value;
};


#endif //BEVA_STATE_H
