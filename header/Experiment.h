//
// Created by berg on 12/07/19.
//

#ifndef BEVA_EXPERIMENT_H
#define BEVA_EXPERIMENT_H

#include <vector>
#include <map>

using namespace std;

class Experiment {
public:
    Experiment(map<string, string>, int);

    map<string, string>  config;
    int editDistanceThreshold;
    vector<long> processingTimes;
    chrono::time_point<std::chrono::system_clock> startIndexingTime;
    chrono::time_point<std::chrono::system_clock> finishIndexingTime;

    chrono::time_point<std::chrono::system_clock> startQueryProcessingTime;
    chrono::time_point<std::chrono::system_clock> finishQueryProcessingTime;

    void initIndexingTime();
    void endIndexingTime();
    void initQueryProcessingTime();
    void endQueryProcessingTime(int);
    void compileQueryProcessingTimes(int);
    void proportionOfBranchingSizeInBEVA2Level();

    void writeFile(const string&, const string&);
};


#endif //BEVA_EXPERIMENT_H
