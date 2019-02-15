#include <iostream>
#include <fstream>
#include "Framework.h"
#include "C.h"

using namespace std;

int main() {
    int editDistanceThreshold = 1;

    Framework* framework = new Framework(editDistanceThreshold);

    string query = "cez";
    string queryRemaining = "ling heigh";

    int count = 0;

    while (query.length() <= 13) {
        framework->process(query, C::BEVA);

        query += queryRemaining[count];
        query += queryRemaining[count + 1];
        count += 2;
    }

    return 0;
}
