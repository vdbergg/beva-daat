#include <utility>

#include <utility>

//
// Created by berg on 12/07/19.
//
#include <chrono>
#include <iostream>
#include <fstream>

#include "../header/Experiment.h"

using namespace std;

Experiment::Experiment(map<string, string> config, int editDistanceThreshold) {
    this->config = std::move(config);
    this->editDistanceThreshold = editDistanceThreshold;
}

void Experiment::writeFile(const string& name, const string& value) {
    ofstream myfile;
    string newName = config["experiments_basepath"] + name;
    newName += "_data_set_" + config["dataset"] + "_size_type_" + config["size_type"] +
            "_tau_" + to_string(this->editDistanceThreshold) + "_alg_BEVA.txt";
    cout << newName << "\n";
    myfile.open(newName);

    if (myfile.is_open()) {
        myfile << value;
        myfile.close();
    } else {
        cout << "Unable to open file.\n";
    }
}

void Experiment::initIndexingTime() {
    this->startIndexingTime = chrono::high_resolution_clock::now();
}

void Experiment::endIndexingTime() {
    this->finishIndexingTime = chrono::high_resolution_clock::now();

    long result = chrono::duration_cast<chrono::microseconds>(
            this->finishIndexingTime - this->startIndexingTime
            ).count();


    writeFile("indexing_time", to_string(result));
}

void Experiment::initQueryProcessingTime() {
    this->startQueryProcessingTime = chrono::high_resolution_clock::now();
}

void Experiment::endQueryProcessingTime(int currentQueryLength) {
    this->finishQueryProcessingTime = chrono::high_resolution_clock::now();

    long result = chrono::duration_cast<chrono::microseconds>(
            this->finishQueryProcessingTime - this->startQueryProcessingTime
    ).count();

    if (this->processingTimes.size() < currentQueryLength   ) {
        this->processingTimes.push_back(result);
    } else {
        this->processingTimes[currentQueryLength - 1] += result;
    }
}

void Experiment::compileQueryProcessingTimes(int countQueryProcessed) {
    string value = "Média de tempo para " + to_string(countQueryProcessed) + " consultas.\n";
    value += "Limiar de distância de edição: " + to_string(this->editDistanceThreshold) + "\n\n";
    int count = 1;

    for (long time : this->processingTimes) {
        long result = time / countQueryProcessed;
        value += to_string(count) + " - " + to_string(result) + "\n";
        count++;
    }
    writeFile("query_processing_time", value);
}

void Experiment::proportionOfBranchingSizeInBEVA2Level() {

}
