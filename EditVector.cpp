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
    this->isInitial = false;
    this->isFinal = false;
}

EditVector::~EditVector() = default;

void EditVector::calculateEditDistance(string query, string data) {
    this->jColumn = (int) data.length();

    if (this->jColumn == 0 || this->previousEditVector == nullptr) {
        buildInitialEditVector();
    } else {
        buildEditVector(query, data);
    }
}

void EditVector::buildInitialEditVector() {
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
    this->editDistance = utils::MARKER;

    this->isInitial = true;
}

void EditVector::buildEditVector(string query, string data) {
    query = " " + query;
    data = " " + data;

    // v_j+1[i] = min(v_j[i] + δ(d[j + 1], Q[j − τ + i]), v_j[i + 1] + 1, v_j+1[i − 1] + 1), ∀1 ≤ i ≤ 2τ + 1.

    this->isFinal = true;

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

        if (this->vector[i] <= this->editDistanceThreshold) {
            this->isFinal = false;
        }
    }
    setEditDistance(query, data);
}

void EditVector::buildEditVectorWithBitmap(string bitmap) {
//    cout << bitmap << "\n";
//    utils::printVector(this->previousEditVector->vector, this->previousEditVector->size);
    this->isFinal = true;
    for (int i = 0; i < this->size; i++) {
        int temp = bitmap[i] - '0';
        temp = temp == 0 ? 1 : 0; // In edit vector construction the ith bitmap is denied
        this->vector[i] = utils::min(
                this->previousEditVector->vector[i] + temp,
                i + 1 >= this->previousEditVector->size ? utils::MARKER : this->previousEditVector->vector[i + 1] + 1,
                i - 1 < 0 ? utils::MARKER : this->vector[i - 1] + 1
        );
        if (this->vector[i] <= this->editDistanceThreshold) {
            this->isFinal = false;
        }
    }
    this->editDistance = this->vector[this->size - 1];
//    utils::printVector(this->vector, this->size);
//    cout << "\n";
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
