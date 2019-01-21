//
// Created by berg on 13/01/19.
//
#include <string>

#ifndef BEVA_EDITDISTANCE_H
#define BEVA_EDITDISTANCE_H

using namespace std;

class EditDistance {
public:
    int** D;
    int m;
    int n;
    int editDistance;
    bool isFilled;

    EditDistance(int, int);
    EditDistance(EditDistance*, int);
    ~EditDistance();

    void calculate(string, string);
    int getEditDistance();
};

#endif //BEVA_EDITDISTANCE_H
