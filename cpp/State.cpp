//
// Created by berg on 28/01/19.
//

#include "../header/State.h"
#include <utility>

State::State(EditVector *editVector, int id) {
    this->editVector = editVector;
    this->isInitial = editVector->isInitial;
    this->isFinal = editVector->isFinal;
    this->id = id;
}

State::~State() {
    delete this->editVector;
};

int State::getEditDistance(string& query, string& data) {
    return this->editVector->getEditDistance(query, data);
}
