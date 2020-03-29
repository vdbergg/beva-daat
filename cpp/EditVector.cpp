//
// Created by berg on 28/01/19.
//

#include "../header/EditVector.h"
#include "../header/utils.h"

EditVector::EditVector(int editDistanceThreshold, EditVector* previousEditVector) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->size = (2 * this->editDistanceThreshold) + 1;
    this->vectorChar = VectorChar(this->size);
    this->previousEditVector = previousEditVector;
}

EditVector::~EditVector() = default;

void EditVector::buildInitialEditVector() {
    unsigned countNegative = this->editDistanceThreshold;
    unsigned countPositive = 1;

    for (int i = 0; i < this->size; i++) {
        if (i < this->editDistanceThreshold) {
            this->set(i, countNegative);
            countNegative--;
        } else if (i == this->editDistanceThreshold) {
            this->set(i, 0);
        } else {
            this->set(i, countPositive);
            countPositive++;
        }
    }
}

void EditVector::buildEditVectorWithBitmap(unsigned bitmap) {
    for (int i = 0; i < this->size; i++) {
        unsigned char item = utils::min(
                this->previousEditVector->get(i) + (1 - utils::getKthBitFromDecimal(bitmap, this->size - 1 - i)),
                i + 1 >= this->previousEditVector->size ? C::MARKER : this->previousEditVector->get(i + 1) + 1,
                i - 1 < 0 ? C::MARKER : this->get(i - 1) + 1
        );
        this->set(i, item);
    }
}
