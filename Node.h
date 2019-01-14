//
// Created by berg on 13/01/19.
//

#ifndef BEVA_NODE_H
#define BEVA_NODE_H

#define CHAR_SIZE 128

class Node {
public:
    bool isEndOfWord;
    Node* children[CHAR_SIZE]; // children
    char value;
    int editDistance;

    Node(char value = '\0');
};

#endif //BEVA_NODE_H
