//
// Created by berg on 09/02/19.
//

#include "../header/ActiveNode.h"

ActiveNode::ActiveNode(Node* node, State* state, string &data) {
    this->node = node;
    this->state = state;
    this->data = data;
}
