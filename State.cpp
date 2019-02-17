//
// Created by berg on 28/01/19.
//

#include "State.h"
#include <utility>

State::State(EditVector *editVector, int id) {
    this->editVector = editVector;
    this->isInitial = editVector->isInitial;
    this->isFinal = editVector->isFinal;
    this->id = id;
}

State::~State() = default;

int State::getEditDistance(string query, string data) {
    return this->editVector->getEditDistance(move(query), move(data));
}
