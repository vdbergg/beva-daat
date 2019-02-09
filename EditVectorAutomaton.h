//
// Created by berg on 28/01/19.
//

#ifndef BEVA_EDITVECTORAUTOMATON_H
#define BEVA_EDITVECTORAUTOMATON_H


#include "State.h"

class EditVectorAutomaton {
public:
    State* initialState;
    State* finalState;
    int editDistanceThreshold;
    int size;

    EditVectorAutomaton(int);

    ~EditVectorAutomaton();

    State* setTransition(State *state, string bitmap);
    void buildAutomaton();
};


#endif //BEVA_EDITVECTORAUTOMATON_H
