//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


#include "Node.h"

class ActiveNode {
public:
    unsigned node;
    State* state;
    unsigned level;

    ActiveNode(unsigned, State*, unsigned);
    ~ActiveNode();
};

#endif //BEVA_ACTIVENODE_H
