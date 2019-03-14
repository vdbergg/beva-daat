//
// Created by berg on 13/01/19.
//

#include "../header/Node.h"

Node::Node(char value, State* state) {
    this->isEndOfWord = false;
    this->value = value;
    this->state = state;
}
