//
// Created by berg on 03/02/19.
//

#include <algorithm>
#include <iostream>
#include "utils.h"

using namespace std;

int utils::min(int a, int b, int c) {
    return std::min(std::min(a, b), c);
}

void utils::printVector(int* vector, int size) {
    for(int i = 0; i < size; ++i) {
        cout << vector[i] << " ";
    }
    cout << "\n";
}

void utils::printTransition(int* from, int sizeFrom, int* to, int sizeTo, string bitmap) {
    cout << "f([ ";
    for(int i = 0; i < sizeFrom; ++i) {
        cout << from[i] << " ";
    }
    cout << "], " + bitmap + ") = [ ";
    for(int i = 0; i < sizeTo; ++i) {
        cout << to[i] << " ";
    }
    cout << "]\n";
}
