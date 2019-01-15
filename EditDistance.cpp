//
// Created by berg on 13/01/19.
//
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

#include "EditDistance.h"

using namespace std;

int min(int lst[]) {
    int min = lst[0];

    for (int i = 1; i < 3; i++) {
        if (lst[i] < min) {
            min = lst[i];
        }
    }
    return min;
}

void print(int** m, int row, int column) {
    for (int i = 0; i < row; ++i) {
        for(int j = 0; j < column; ++j) {
            cout << m[i][j] << " ";
        }
        cout << "\n";
    }
}

int EditDistance::editDistance(string s, string t) {
    s = " " + s;
    t = " " + t;
    int** m = new int *[s.length()];

    for (int i = 0; i < s.length(); i++) {
	    m[i] = new int[t.length()]; // columns
    }

    for (int i = 0; i < s.length(); i++) {
        m[i][0] = i;
    }

    for (int j = 0; j < t.length(); j++) {
        m[0][j] = j;
    }

    for (int i = 1; i < s.length(); i++) {
        for (int j = 1; j < t.length(); j++) {
            // M[i, j] = min(M[i − 1, j − 1] + δ(d[j], Q[i]), M[i − 1, j] + 1, M[i, j − 1] + 1)
            // δ(x, y) = 0 if x = y or 1 otherwise
            int lst[3] = { m[i - 1][j - 1] + (t.at(j) == s.at(i) ? 0 : 1), m[i - 1][j] + 1, m[i][j - 1] + 1 };
            m[i][j] = min(lst);
        }
    }

    int editDistance = m[s.length() - 1][t.length() - 1];

    // free
    for (int i = 0; i < s.length(); ++i) {
        delete [] m[i];
    }
    delete [] m;

    return editDistance;
}
