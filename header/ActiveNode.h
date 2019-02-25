//
// Created by berg on 09/02/19.
//

#ifndef BEVA_ACTIVENODE_H
#define BEVA_ACTIVENODE_H


#include "Node.h"

class ActiveNode {
public:
    Node* node;
    string data;

    ActiveNode(Node*, string);
};


#endif //BEVA_ACTIVENODE_H
