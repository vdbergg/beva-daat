//
// Created by berg on 03/02/19.
//

#ifndef BEVA_UTILS_H
#define BEVA_UTILS_H

using namespace std;

class utils {
public:
    static int min(int, int, int);
    static void printVector(int*, int);
    static void printTransition(int*, int, int*, int, string);
    static string shiftBitInBinaryString(string bitmap, int countShiftBit, int bitmapSize);
    static string convertDecimalToBinaryString(int, int);
};


#endif //BEVA_UTILS_H