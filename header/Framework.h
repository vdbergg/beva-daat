//
// Created by vdberg on 12/02/19.
//

#ifndef BEVA_FRAMEWORK_H
#define BEVA_FRAMEWORK_H

#include <vector>
#include <string>
#include <map>
#include "ActiveNode.h"
#include "Trie.h"
#include "Beva.h"
#include "Experiment.h"

using namespace std;

class Framework {
public:
    Trie* trie;
    vector<string> records;
    vector<string> queries;
    vector<ActiveNode*> activeNodes;
    int editDistanceThreshold;
    int dataset;
    Experiment* experiment;

    Beva* beva;

    Framework(map<string,string>);

    void index(map<string,string>);
    void process(string, int, int, int, int);
    void output();

    ~Framework();
};


#endif //BEVA_FRAMEWORK_H
