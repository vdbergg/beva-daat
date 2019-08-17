//
// Created by berg on 13/01/19.
//

#include "../header/Node.h"

Node::Node(char value) {
    this->isEndOfWord = false;
    this->value = value;
    this->beginRange = -1;
    this->endRange = -1;
}
