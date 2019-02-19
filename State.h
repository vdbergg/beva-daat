//
// Created by berg on 28/01/19.
//

#ifndef BEVA_STATE_H
#define BEVA_STATE_H

#include <map>
#include "EditVector.h"

using namespace std;

class State {
public:
    State(EditVector*, int);
    ~State();
    int getEditDistance(string, string);

    EditVector* editVector;
    map<string, State*> transitions;
    bool isInitial;
    bool isFinal;
    int id;
};


#endif //BEVA_STATE_H
