//
// Created by berg on 13/01/19.
//

#ifndef BEVA_NODE_H
#define BEVA_NODE_H

#include <vector>
#include "EditVector.h"
#include "State.h"

#define CHAR_SIZE 128

class Node {
public:
    vector<Node*> children;

    bool isEndOfWord;
    char value;
    int beginRange;
    int endRange;

    Node(char value = '\0');
};

#endif //BEVA_NODE_H
