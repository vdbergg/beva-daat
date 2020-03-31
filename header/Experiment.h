//
// Created by berg on 12/07/19.
//

#ifndef BEVA_EXPERIMENT_H
#define BEVA_EXPERIMENT_H

#include <vector>
#include <chrono>

#include "sys/types.h"

using namespace std;

class Experiment {
public:
    Experiment(unordered_map<string, string>, int);

    unordered_map<string, string> config;
    int editDistanceThreshold;
    long numberOfNodes;
    vector<float> memoryUsedInProcessing;
    vector<long> processingTimes;
    vector<long> fetchingTimes;
    vector<float> resultsSize;
    vector<long> currentQueryProcessingTime;
    vector<long> currentQueryFetchingTime;
    vector<long> currentResultsSize;
    vector<float> activeNodesSizes;
    vector<long> currentActiveNodesSize;
    unordered_map<int, int> branchSize;

    bool recoveryMode = false;

    chrono::high_resolution_clock::time_point startIndexingTime;
    chrono::high_resolution_clock::time_point finishIndexingTime;
    chrono::high_resolution_clock::time_point startQueryProcessingTime;
    chrono::high_resolution_clock::time_point finishQueryProcessingTime;
    chrono::high_resolution_clock::time_point startQueryFetchingTime;
    chrono::high_resolution_clock::time_point finishQueryFetchingTime;

    void initIndexingTime();
    void endIndexingTime();
    void initQueryProcessingTime();
    void endQueryProcessingTime(long, string&);
    void initQueryFetchingTime();
    void endQueryFetchingTime(string&, int, long);
    void compileQueryProcessingTimes(int);
    void proportionOfBranchingSizeInBEVA2Level(int);
    void proportionOfBranchingSize(int size);
    void incrementNumberOfNodes();
    void compileNumberOfNodes();
    void compileProportionOfBranchingSizeInBEVA2Level();
    void readQueryProcessingTime(string& filename);
    void saveQueryProcessingTime(string&, int);
    void getMemoryUsedInIndexing();
    void getMemoryUsedInProcessing(int);

    void writeFile(const string&, const string&, bool writeInTheEnd=false);
};


#endif //BEVA_EXPERIMENT_H
