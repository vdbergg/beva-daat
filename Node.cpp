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
    this->editVector = nullptr;
    this->parent = parent;
}

int Node::getEditDistance() {
//    return this->editDistance->getEditDistance();
    return this->editVector->getEditDistance();
}

void Node::calculateEditDistance(string queryOriginal, string query, int editDistanceThreshold) {
    if (this->parent == nullptr || this->parent->editVector == nullptr) {
//        this->editDistance = new EditDistance((int) queryOriginal.length(), (int) query.length(), editDistanceThreshold);
        this->editVector = new EditVector(editDistanceThreshold, nullptr);
    } else {
//        this->editDistance = new EditDistance(this->parent->editDistance, (int) query.length());
        this->editVector = new EditVector(editDistanceThreshold, this->parent->editVector);
    }

//    this->editDistance->calculateEfficiently(queryOriginal, query);
    this->editVector->calculateEditDistance(queryOriginal, query);
}
