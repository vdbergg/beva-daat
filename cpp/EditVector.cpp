//
// Created by berg on 28/01/19.
//

#include <iostream>
#include "../header/EditVector.h"
#include "../header/utils.h"
#include "../header/C.h"

EditVector::EditVector(int editDistanceThreshold, EditVector* previousEditVector) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->size = (2 * this->editDistanceThreshold) + 1;
    this->vector = new int[this->size];
    this->previousEditVector = previousEditVector;
    this->isInitial = false;
    this->isFinal = false;
    this->value = "";
}

EditVector::~EditVector() = default;

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
        this->value += this->vector[i] > this->editDistanceThreshold ? "#" : to_string(this->vector[i]) + " ";
    }
    this->value.erase(this->value.length() - 1);
    this->isInitial = true;
}

void EditVector::buildEditVectorWithBitmap(string bitmap, string initialStateValue) {
    this->isFinal = true;
    for (int i = 0; i < this->size; i++) {
        int temp = bitmap[i] - '0';
        temp = temp == 0 ? 1 : 0; // In edit vector build the ith bitmap is denied
        this->vector[i] = utils::min(
                this->previousEditVector->vector[i] + temp,
                i + 1 >= this->previousEditVector->size ? C::MARKER : this->previousEditVector->vector[i + 1] + 1,
                i - 1 < 0 ? C::MARKER : this->vector[i - 1] + 1
        );
        if (this->vector[i] <= this->editDistanceThreshold) {
            this->isFinal = false;
        }
        this->value += (this->vector[i] > this->editDistanceThreshold ? "#" : to_string(this->vector[i])) + " ";
    }
    this->value.erase(this->value.length() - 1);
    if (this->value == initialStateValue) {
        this->isInitial = true;
    }
}

int EditVector::getEditDistance(string query, string data) {
    if ((int) query.length() >= ((int) data.length() - this->editDistanceThreshold)
        && (int) query.length() <= ((int) data.length() + this->editDistanceThreshold)) {
        return this->vector[(this->editDistanceThreshold + 1 + ((int) query.length() - (int) data.length())) - 1];
    }
    return C::MARKER;
}
