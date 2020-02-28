//
// Created by berg on 12/07/19.
//

#ifndef BEVA_EXPERIMENT_H
#define BEVA_EXPERIMENT_H

#include <vector>
#include <map>
#include <chrono>
#include "QueryResult.h"

#include "sys/types.h"
#include "sys/sysinfo.h"

using namespace std;

class Experiment {
public:
    Experiment(map<string, string>, int);

    map<string, string>  config;
    int editDistanceThreshold;
    long numberOfNodes;
    vector<float> memoryUsedInProcessing;
    vector<long> processingTimes;
    vector<long> fetchingTimes;
    vector<long> currentQueryProcessingTime;
    vector<long> currentQueryFetchingTime;
    vector<float> activeNodesSizes;
    vector<long> currentActiveNodesSize;
    map<int, int> branchSize;

    bool recoveryMode = false;

    chrono::time_point<std::chrono::system_clock> startIndexingTime;
    chrono::time_point<std::chrono::system_clock> finishIndexingTime;

    chrono::time_point<std::chrono::system_clock> startQueryProcessingTime;
    chrono::time_point<std::chrono::system_clock> finishQueryProcessingTime;

    chrono::time_point<std::chrono::system_clock> startQueryFetchingTime;
    chrono::time_point<std::chrono::system_clock> finishQueryFetchingTime;

    void initIndexingTime();
    void endIndexingTime();
    void initQueryProcessingTime();
    void endQueryProcessingTime(long, string&);
    void initQueryFetchingTime();
    void endQueryFetchingTime(string&, int);
    void compileQueryProcessingTimes(int);
    void proportionOfBranchingSizeInBEVA2Level(int);
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
