//
// Created by berg on 03/02/19.
//

#ifndef BEVA_UTILS_H
#define BEVA_UTILS_H

#include <vector>

using namespace std;

class utils {
public:
    static vector<string> split(string, char);
    static int min(int, int, int);
    static void printVector(int*, int);
    static void printTransition(int*, int, int*, int, string&);
    static string shiftBitInBinaryString(string&, int, int);
    static string convertDecimalToBinaryString(int, int);
    static char convertSpecialCharToSimpleChar(char);
    static string normalize(string&);

    static void printBitsFromDecimal(unsigned decimal);
    static unsigned leftShiftBitInDecimal(unsigned decimal, unsigned short countShiftBit,
            unsigned short int bitmapSize);
    static unsigned char getKthBitFromDecimal(unsigned int decimal, unsigned short k);
    static unsigned int setKthBitFromDecimal(unsigned int decimal, unsigned short k, unsigned short bitmapSize);
};


#endif //BEVA_UTILS_H
