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
    int editDistance;
    int size;
    int jColumn;
    bool isInitial;
    bool isFinal;

    void buildInitialEditVector();
    void buildEditVector(string, string);
    void buildEditVectorWithBitmap(string);
    void calculateEditDistance(string, string);
    int getEditDistance();
    void setEditDistance(string, string);
};


#endif //BEVA_EDITVECTOR_H
