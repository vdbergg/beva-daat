#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include "header/Framework.h"
#include "header/C.h"

using namespace std;

int editDistanceThreshold;
int algorithmType;
int datasetNumber;
int sizeType;

void checkParameters(int argc, char** argv);

int main(int argc, char** argv) {
    checkParameters(argc,argv);
    Framework* framework = new Framework(editDistanceThreshold, datasetNumber, sizeType);

    for (string q : framework->queries) {

        string query = q.substr(0, 1);
        string queryRemaining = q.substr(1);

        int count = 0;

        while (query.length() <= q.length()) {
            framework->process(query, algorithmType, (int) q.length());

            query += queryRemaining[count];
            count++;
            this_thread::sleep_for(chrono::seconds(1));
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
        sizeType = 3;
    }
    if (number == 5) {
        editDistanceThreshold = atoi(values[1]);
        algorithmType = atoi(values[2]);
        datasetNumber = atoi(values[3]);
        sizeType      = atoi(values[4]);
    }
    else {
        std::cout << "[ERROR] Missing parameters" << std::endl << std::endl;
        std::cout << "./beva EDIT_DIST ALG_TYPE DATASET [SIZE_TYPE]" << std::endl;
        std::cout << "EDIT_DIST -> edit distance number." << std::endl;
        std::cout << "ALG_TYPE -> ." << std::endl;
        std::cout << "\t BEVA -> 0" << std::endl;
        std::cout << "DATASET -> dataset number." << std::endl;
        std::cout << "\t AOL -> 0" << std::endl;
        std::cout << "\t MEDLINE -> 1" << std::endl;
        std::cout << "\t USADDR -> 2" << std::endl;
        std::cout << "SIZE_TYPE -> percentual of dataset to be loaded." << std::endl;
        std::cout << "\t 25% -> 0" << std::endl;
        std::cout << "\t 50% -> 1" << std::endl;
        std::cout << "\t 75% -> 2" << std::endl;
        std::cout << "\t 100% -> 3" << std::endl;

        exit(1);
    }
}