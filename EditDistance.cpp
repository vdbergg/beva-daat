//
// Created by berg on 13/01/19.
//
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

#include "EditDistance.h"

using namespace std;

void print(int** D, int m, int n) {
    for (int i = 0; i < m; ++i) {
        for(int j = 0; j < n; ++j) {
            cout << D[i][j] << " ";
        }
        cout << "\n";
    }
}

EditDistance::EditDistance(int m, int n) {
    this->m = m + 1;
    this->n = n + 1;
    this->isFilled = false;
    this->editDistance = 30;

    this->D = new int *[this->m];

    for (int i = 0; i < this->m; i++) {
        this->D[i] = new int[this->n]; // columns
    }
}

EditDistance::EditDistance(EditDistance* editDistance, int n) {
    this->m = editDistance->m;
    this->n = n + 1;
    this->isFilled = editDistance->isFilled;
    this->editDistance = editDistance->editDistance;

    int** temp = editDistance->D;
    this->D = new int *[this->m];

    for (int i = 0; i < this->m; i++) {
        this->D[i] = new int[this->n]; // columns
    }

    for (int i = 0; i < this->m; i++) {
        for (int j = 0; j < this->n; j++) {
            this->D[i][j] = temp[i][j];
        }
    }
}

EditDistance::~EditDistance() {
    for (int i = 0; i < this->m; ++i) {
        delete [] this->D[i];
    }
    delete [] this->D;
}

int min(int a, int b, int c) {
    return min(min(a, b), c);
}

void EditDistance::calculate(string s, string t) {
    int temp = 0;
    if (this->isFilled) temp = this->n - 1;
    else isFilled = true;

    for (int i = 0; i < this->m; i++) {
        for (int j = temp; j < this->n; j++) {
            if (i == 0) this->D[i][j] = j;
            else if (j == 0) this->D[i][j] = i;
            else if (t.at(j - 1) == s.at(i - 1)) this->D[i][j] = this->D[i - 1][j - 1];
            else this->D[i][j] = min(this->D[i - 1][j - 1], this->D[i - 1][j], this->D[i][j - 1]) + 1;
        }
    }
    this->editDistance = this->D[this->m - 1][this->n - 1];
}

int EditDistance::getEditDistance() {
    return this->editDistance;
}
