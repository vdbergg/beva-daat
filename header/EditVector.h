//
// Created by berg on 28/01/19.
//

#include <string>

#ifndef BEVA_EDITVECTOR_H
#define BEVA_EDITVECTOR_H

using namespace std;

class EditVector {
public:
    EditVector(int, EditVector*);
    ~EditVector();

    EditVector* previousEditVector;
    unsigned char* vector;
    int editDistanceThreshold;
    int size;

    void buildInitialEditVector();
    void buildEditVectorWithBitmap(unsigned);
    int getEditDistance(int);
    string getEditVectorSignature();
};

#endif //BEVA_EDITVECTOR_H
