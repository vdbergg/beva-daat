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
