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
    ~Experiment();

    unordered_map<string, string> config;
    int editDistanceThreshold;
    long numberOfNodes;
    vector<unsigned long> numberOfActiveNodes;
    vector<unsigned long> numberOfIterationInChildren;
    vector<long> processingTimes;
    vector<long> fetchingTimes;
    vector<long> resultsSize;
    vector<long> currentQueryProcessingTime;
    vector<long> currentQueryFetchingTime;
    vector<long> currentResultsSize;
    vector<long> activeNodesSizes;
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
    void endQueryProcessingTime(long, int);
    void initQueryFetchingTime();
    void endQueryFetchingTime(int, unsigned long);
    void compileQueryProcessingTimes(int);
    void proportionOfBranchingSize(int size);
    void incrementNumberOfNodes();
    void incrementNumberOfActiveNodes(unsigned);
    void compileNumberOfActiveNodes();
    void incrementNumberOfIterationInChildren(unsigned);
    void compileNumberOfIterationInChildren();
    void compileNumberOfNodes();
    void compileProportionOfBranchingSizeInBEVA2Level();
    void readQueryProcessingTime(string& filename);
    void saveQueryProcessingTime(string&, int);
    void getMemoryUsedInIndexing();
    void getMemoryUsedInProcessing();

    void writeFile(const string&, const string&, bool writeInTheEnd=false);
};


#endif //BEVA_EXPERIMENT_H
