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
    int* vector;
    int editDistanceThreshold;
    int size;
    bool isInitial;
    bool isFinal;
    string value;

    void buildInitialEditVector();
    void buildEditVectorWithBitmap(string, string);
    int getEditDistance(string, string);
};

#endif //BEVA_EDITVECTOR_H
