//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


#include "State.h"

class ActiveNode {
public:
    State* state;
    unsigned node;
    unsigned level;
    unsigned editDistance;

    ActiveNode(unsigned, State*, unsigned, unsigned);
    ActiveNode(unsigned, State*, unsigned);
};

#endif //BEVA_ACTIVENODE_H
