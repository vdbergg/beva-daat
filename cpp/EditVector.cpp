//
// Created by berg on 28/01/19.
//

#include "../header/EditVector.h"
#include "../header/utils.h"
#include "../header/C.h"

EditVector::EditVector(int editDistanceThreshold, EditVector* previousEditVector) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->size = (2 * this->editDistanceThreshold) + 1;
    this->vector = new int[this->size];
    this->previousEditVector = previousEditVector;
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
    }
}

void EditVector::buildEditVectorWithBitmap(unsigned bitmap) {
    for (int i = 0; i < this->size; i++) {
        char bit = utils::getKthBitFromDecimal(bitmap, this->size - 1 - i) == 0 ? 1 : 0; // In edit vector build the ith bitmap is denied
        this->vector[i] = utils::min(
                this->previousEditVector->vector[i] + bit,
                i + 1 >= this->previousEditVector->size ? C::MARKER : this->previousEditVector->vector[i + 1] + 1,
                i - 1 < 0 ? C::MARKER : this->vector[i - 1] + 1
        );
    }
}

int EditVector::getEditDistance(int k) {
    int result = (this->editDistanceThreshold + 1 + k) - 1;
    if (result < this->size) return this->vector[result];
    return C::MARKER;
}

string EditVector::getEditVectorSignature() {
    string value;
    for (int i = 0; i < this->size; i++) value += to_string(this->vector[i]);
    return value;
}
