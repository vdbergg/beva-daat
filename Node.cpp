//
// Created by berg on 13/01/19.
//

#include "Node.h"

Node::Node(char value, State* state) {
    this->isEndOfWord = false;
    for (int i = 0; i < CHAR_SIZE; i++) {
        this->children[i] = nullptr;
    }
    this->value = value;
    this->state = state;
}
