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
    vector<long> processingTimes;
    vector<long> currentQueryProcessingTime;
    vector<float> activeNodesSizes;
    vector<long> currentActiveNodesSize;
    map<int, int> branchSize;

    bool recoveryMode = false;

    vector<pair<string, QueryResult*>> processingTimeByQuery;
    chrono::time_point<std::chrono::system_clock> startIndexingTime;
    chrono::time_point<std::chrono::system_clock> finishIndexingTime;

    chrono::time_point<std::chrono::system_clock> startQueryProcessingTime;
    chrono::time_point<std::chrono::system_clock> finishQueryProcessingTime;

    void initIndexingTime();
    void endIndexingTime();
    void initQueryProcessingTime();
    void endQueryProcessingTime(long, string&, int);
    void compileQueryProcessingTimes(int);
    void proportionOfBranchingSizeInBEVA2Level(int);
    void incrementNumberOfNodes();
    void compileNumberOfNodes();
    void compileLongAndShortProcessingTimeQueries(int);
    void compileProportionOfBranchingSizeInBEVA2Level();
    void readQueriesProcessingTime(string& filename);
    void readQueryProcessingTime(string& filename);

    void writeFile(const string&, const string&);
};


#endif //BEVA_EXPERIMENT_H
