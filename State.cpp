//
// Created by berg on 28/01/19.
//

#include "State.h"

State::State(EditVector *editVector, int id) {
    this->editVector = editVector;
    this->isInitial = editVector->isInitial;
    this->isFinal = editVector->isFinal;
    this->id = id;
}

State::~State() = default;
