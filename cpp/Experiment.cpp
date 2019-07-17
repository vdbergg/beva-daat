#include <utility>

#include <utility>

//
// Created by berg on 12/07/19.
//
#include <iostream>
#include <fstream>

#include "../header/Experiment.h"

using namespace std;

Experiment::Experiment(map<string, string> config, int editDistanceThreshold) {
    this->config = std::move(config);
    this->editDistanceThreshold = editDistanceThreshold;

    for (int i = 0; i < 13; i++) {
        this->processingTimes.push_back(0);
    }
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

    this->processingTimes[currentQueryLength - 1] += result;
}

void Experiment::compileQueryProcessingTimes(int countQueryProcessed) {
    string value = "Média de tempo para " + to_string(countQueryProcessed) + " consultas.\n";
    value += "Limiar de distância de edição: " + to_string(this->editDistanceThreshold) + "\n\n";

    int accum = 0;
    for (int i = 0; i < this->processingTimes.size(); i++) {
        this->processingTimes[i] = this->processingTimes[i] / countQueryProcessed;
        accum += this->processingTimes[i];
        value += to_string(i + 1) + " - " + to_string(this->processingTimes[i]) +
                " acumulado: " + to_string(accum) + "\n";
    }

    writeFile("query_processing_time", value);
}

void Experiment::proportionOfBranchingSizeInBEVA2Level(int size) {
    if (this->branchSize.find(size) == this->branchSize.end() ) {
        this->branchSize[size] = 1;
    } else {
        this->branchSize[size]++;
    }
}

void Experiment::compileProportionOfBranchingSizeInBEVA2Level() {
    string value = "";
    for (map<int, int>::iterator it = this->branchSize.begin(); it != this->branchSize.end(); ++it) {
        value += to_string(it->first) + " " + to_string(it->second) + "\n";
    }
    writeFile("proportion_branch_size_beva_2_level", value);
}
