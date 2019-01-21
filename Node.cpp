//
// Created by berg on 13/01/19.
//

#include "Node.h"

Node::Node(char value, Node* parent) {
    this->isEndOfWord = false;
    for (int i = 0; i < CHAR_SIZE; i++) {
        this->children[i] = nullptr;
    }
    this->value = value;
    this->editDistance = nullptr;
    this->parent = parent;
}

int Node::getEditDistance() {
    return this->editDistance->getEditDistance();
}

void Node::calculateEditDistance(string queryOriginal, string query) {
    if (this->parent == nullptr || this->parent->editDistance == nullptr) {
        this->editDistance = new EditDistance((int) queryOriginal.length(), (int) query.length());
    } else {
        this->editDistance = new EditDistance(this->parent->editDistance, (int) query.length());
    }

    this->editDistance->calculate(queryOriginal, query);
}
