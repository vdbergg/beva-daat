//
// Created by berg on 03/02/19.
//

#ifndef BEVA_UTILS_H
#define BEVA_UTILS_H

#include "QueryResult.h"

using namespace std;

class utils {
public:
    static vector<string> split(string, char);
    static int min(int, int, int);
    static bool sortQueryProcessingTime(const pair<string, QueryResult*>&, const pair<string, QueryResult*>&);
    static void printVector(int*, int);
    static void printTransition(int*, int, int*, int, string&);
    static string shiftBitInBinaryString(string&, int, int);
    static string convertDecimalToBinaryString(int, int);
    static char convertSpecialCharToSimpleChar(char);
    static string normalize(string&);
};


#endif //BEVA_UTILS_H
