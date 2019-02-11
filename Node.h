//
// Created by berg on 13/01/19.
//

#ifndef BEVA_NODE_H
#define BEVA_NODE_H

#include <vector>
#include "EditDistance.h"
#include "EditVector.h"

#define CHAR_SIZE 128

class Node {
public:
    Node* parent;
    Node* children[CHAR_SIZE];
    EditDistance* editDistance;
    EditVector* editVector;

    bool isEndOfWord;
    char value;
    vector<int> recordsId;

    Node(char value = (char) 0, Node* = nullptr);
    int getEditDistance();
    void calculateEditDistance(string, string, int);
};

#endif //BEVA_NODE_H
