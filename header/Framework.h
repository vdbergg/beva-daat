//
// Created by vdberg on 12/02/19.
//

#ifndef BEVA_FRAMEWORK_H
#define BEVA_FRAMEWORK_H

#include <vector>
#include <string>
#include "ActiveNode.h"
#include "Trie.h"
#include "Beva.h"

using namespace std;

class Framework {
public:
    Trie* trie;
    vector<string> records;
    vector<string> queries;
    vector<ActiveNode*> activeNodes;
    int editDistanceThreshold;
    int dataset;

    Beva* beva;

    Framework(int, int, int);

    void index(int);
    void process(string, int, int);
    void output();

    ~Framework();
};


#endif //BEVA_FRAMEWORK_H
