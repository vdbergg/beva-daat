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

    Beva* beva;

    static const int BEVA = 0;

    Framework(int);

    void index();
    void process(string, int);
    void output();

    ~Framework();
};


#endif //BEVA_FRAMEWORK_H
