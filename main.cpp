#include <iostream>
#include <fstream>
#include "header/Framework.h"
#include "header/C.h"

using namespace std;

int editDistanceThreshold;
int algorithmType;
int datasetNumber;

void checkParameters(int argc, char** argv);

int main(int argc, char** argv) {
    checkParameters(argc,argv);
    Framework* framework = new Framework(editDistanceThreshold, datasetNumber);

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

    delete framework;
    return 0;
}

void checkParameters(int number, char** values) {
    if (number == 4) {
        editDistanceThreshold = atoi(values[1]);
        algorithmType = atoi(values[2]);
        datasetNumber = atoi(values[3]);
    } else {
        std::cout << "[ERROR] Missing parameters" << std::endl << std::endl;
        std::cout << "./beva EDIT_DIST ALG_TYPE DATASET" << std::endl;
        std::cout << "EDIT_DIST -> edit distance number." << std::endl;
        std::cout << "ALG_TYPE -> ." << std::endl;
        std::cout << "\t BEVA -> 0" << std::endl;
        std::cout << "DATASET -> dataset number." << std::endl;
        std::cout << "\t AOL -> 0" << std::endl;
        std::cout << "\t MEDLINE -> 1" << std::endl;
        std::cout << "\t USADDR -> 2" << std::endl;

        exit(1);
    }
}