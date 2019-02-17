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
    Node* children[CHAR_SIZE];
    State* state;

    bool isEndOfWord;
    char value;
    vector<int> recordsId;

    Node(char value = (char) 0, State* = nullptr);
};

#endif //BEVA_NODE_H
