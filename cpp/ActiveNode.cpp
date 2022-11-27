//
// Created by berg on 09/02/19.
//

#include "../header/ActiveNode.h"

ActiveNode::ActiveNode(unsigned node, State* state, unsigned level, unsigned editDistance) {
    this->node = node;
    this->state = state;
    this->level = level;
    this->editDistance = editDistance;
}

ActiveNode::ActiveNode(unsigned node, State* state, unsigned level) {
    this->node = node;
    this->state = state;
    this->level = level;
}
