//
// Created by berg on 13/01/19.
//
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

#include "EditDistance.h"

using namespace std;

int min(int a, int b, int c) {
    return min(min(a, b), c);
}

void print(int** m, int row, int column) {
    for (int i = 0; i < row; ++i) {
        for(int j = 0; j < column; ++j) {
            cout << m[i][j] << " ";
        }
        cout << "\n";
    }
}

int editDistance(string s, string t) {
    s = " " + s;
    t = " " + t;
    long m = s.length();
    long n = t.length();

    int D[m][n];

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0) D[i][j] = j;
            else if (j == 0) D[i][j] = i;
            else if (t.at(j) == s.at(i)) D[i][j] = D[i - 1][j - 1];
            else D[i][j] = min(D[i - 1][j - 1], D[i - 1][j], D[i][j - 1]) + 1;
        }
    }

    return D[m - 1][n - 1];
}
