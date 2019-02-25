#include <iostream>
#include <fstream>
#include "header/Framework.h"
#include "header/C.h"

using namespace std;

int main(int argc, char** argv) {

    int editDistanceThreshold = atoi(argv[1]);
    int algorithmType = atoi(argv[2]);
    int dataset = atoi(argv[3]);

    Framework* framework = new Framework(editDistanceThreshold, dataset);

    for (string q : framework->queries) {

        string query = q.substr(0, 1);
        string queryRemaining = q.substr(1);

        int count = 0;

        while (query.length() <= q.length()) {
            framework->process(query, algorithmType, (int) q.length());

            query += queryRemaining[count];
            count++;
        }
    }

    return 0;
}
