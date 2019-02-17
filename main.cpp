#include <iostream>
#include <fstream>
#include "Framework.h"
#include "C.h"

using namespace std;

int main() {
    int editDistanceThreshold = 1;

    Framework* framework = new Framework(editDistanceThreshold);

    string query = "c";
    string queryRemaining = "ezling heigh";

    int count = 0;

    while (query.length() <= 13) {
        framework->process(query, C::BEVA);

        query += queryRemaining[count];
        count++;
    }

    return 0;
}
