//
// Created by berg on 03/02/19.
//

#include <algorithm>
#include <iostream>
#include <bitset>
#include "../header/utils.h"

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

string utils::shiftBitInBinaryString(string bitmap, int countShiftBit, int bitmapSize) {
    int decimal = (int) bitset<16>(bitmap).to_ulong();
    decimal = decimal << countShiftBit; // shifting 1 bit leftwards
    bitmap = bitset<16>(decimal).to_string();
    bitmap = bitmap.substr(bitmap.length() - bitmapSize);

    return bitmap;
}

string utils::convertDecimalToBinaryString(int decimal, int bitmapSize) {
    string bitmap = bitset<16>(decimal).to_string();
    return bitmap.substr(bitmap.length() - bitmapSize);
}
