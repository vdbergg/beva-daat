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
    this->editVector = nullptr;
    this->parent = parent;
}

int Node::getEditDistance() {
    return this->editVector->getEditDistance();
}

void Node::calculateEditDistance(string queryOriginal, string query, int editDistanceThreshold) {
    if (this->parent == nullptr || this->parent->editVector == nullptr) {
        this->editVector = new EditVector(editDistanceThreshold, nullptr);
    } else {
        this->editVector = new EditVector(editDistanceThreshold, this->parent->editVector);
    }

    this->editVector->calculateEditDistance(queryOriginal, query);
}
