#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include "header/Framework.h"
#include "header/C.h"

using namespace std;

int editDistanceThreshold;
int qryNumber;
int datasetNumber;
int sizeType;

void checkParameters(int argc, char** argv);

int main(int argc, char** argv) {
    checkParameters(argc,argv);
    string q, query, queryRemaining;
    Framework* framework = new Framework(editDistanceThreshold, datasetNumber, sizeType);
    int indexMin, indexMax;
    int algorithmType = 0; // = -> BEVA

    if(qryNumber == -1){
        indexMin = 0;
        indexMax = framework->queries.size();
    }
    else{
        indexMin = qryNumber;
        indexMax = qryNumber + 1;
    }

    for (int i = indexMin; i < indexMax; ++i) {
        q = framework->queries[i];
        query = q.substr(0, 1);
        queryRemaining = q.substr(1);

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
        qryNumber = atoi(values[2]);
        datasetNumber = atoi(values[3]);
        sizeType = 3;
    }
    else if (number == 5) {
        editDistanceThreshold = atoi(values[1]);
        qryNumber = atoi(values[2]);
        datasetNumber = atoi(values[3]);
        sizeType      = atoi(values[4]);
    }
    else {
        std::cout << "[ERROR] Missing parameters" << std::endl << std::endl;
        std::cout << "./beva EDIT_DIST QRY_NUMBER DATASET [SIZE_TYPE]" << std::endl;
        std::cout << "EDIT_DIST -> edit distance number." << std::endl;
        std::cout << "QRY_NUMBER -> query number starting from 0" << std::endl;
        std::cout << "\t ALL -> -1" << std::endl;
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