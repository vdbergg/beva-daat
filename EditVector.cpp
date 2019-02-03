//
// Created by berg on 28/01/19.
//

#include <iostream>
#include "EditVector.h"
#include "utils.h"

EditVector::EditVector(int editDistanceThreshold, EditVector* previousEditVector) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->size = (2 * this->editDistanceThreshold) + 1;
    this->vector = new int[this->size];
    this->editDistance = utils::MARKER;
    this->previousEditVector = previousEditVector;
}

EditVector::~EditVector() = default;

void EditVector::calculateEditDistance(string query, string data) {
    this->jColumn = (int) data.length();

    if (this->jColumn == 0 || this->previousEditVector == nullptr) {
        buildInitialEditVector(query, data);
    } else {
        buildEditVector(query, data);
    }
}

void EditVector::buildInitialEditVector(string query, string data) {
    int countNegative = this->editDistanceThreshold;
    int countPositive = 1;

    for (int i = 0; i < this->size; i++) {
        if (i < this->editDistanceThreshold) {
            this->vector[i] = countNegative;
            countNegative--;
        } else if (i == this->editDistanceThreshold) {
            this->vector[i] = 0;
        } else {
            this->vector[i] = countPositive;
            countPositive++;
        }
    }
    setEditDistance(query, data);

    this->isInitial = true;
}

void EditVector::buildEditVector(string query, string data) {
    query = " " + query;
    data = " " + data;

    // v_j+1[i] = min(v_j[i] + δ(d[j + 1], Q[j − τ + i]), v_j[i + 1] + 1, v_j+1[i − 1] + 1), ∀1 ≤ i ≤ 2τ + 1.

    for (int i = 0; i < this->size; i++) {
        int indexQuery = this->jColumn - this->editDistanceThreshold + i;

        if (indexQuery >= 0 && indexQuery < query.length()) {
            int temp = query.at(indexQuery) == data.at(this->jColumn) ? 0 : 1;
            this->vector[i] = utils::min(
                    this->previousEditVector->vector[i] + temp,
                    i + 1 >= this->previousEditVector->size ? utils::MARKER : this->previousEditVector->vector[i + 1] + 1,
                    i - 1 < 0 ? utils::MARKER : this->vector[i - 1] + 1
            );
        } else {
            this->vector[i] = utils::MARKER;
        }
    }
    setEditDistance(query, data);

    if (this->vector[0] > this->editDistanceThreshold) {
        this->isFinal = true;
    }
}

int EditVector::getEditDistance() {
    return this->editDistance;
}

void EditVector::setEditDistance(string query, string data) {
    if (query.length() >= data.length() - this->editDistanceThreshold
        && query.length() <= data.length() + this->editDistanceThreshold) {
        this->editDistance = this->vector[(this->editDistanceThreshold + 1 + (query.length() - data.length())) - 1];
    } else {
        this->editDistance = utils::MARKER;
    }
}
