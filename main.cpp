#include <iostream>
#include <fstream>
#include "Framework.h"

using namespace std;

int main() {

    Framework* framework = new Framework();

    string query = "cez";
    string queryRemaining = "ling heigh";

    int count = 0;
    int editDistanceThreshold = 1;

    while (query.length() <= 13) {
        framework->process(query, editDistanceThreshold);

        query += queryRemaining[count];
        query += queryRemaining[count + 1];
        count += 2;
    }

    return 0;
}
