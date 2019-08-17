//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


#include "Node.h"

class ActiveNode {
public:
    Node* node;
    State* state;
    string data;

    ActiveNode(Node*, State*, string&);
};


#endif //BEVA_ACTIVENODE_H
