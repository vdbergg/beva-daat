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
    string data;

    ActiveNode(unsigned, State*, string&);
    ~ActiveNode();
};


#endif //BEVA_ACTIVENODE_H
