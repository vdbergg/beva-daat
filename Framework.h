//
// Created by vdberg on 12/02/19.
//

#ifndef BEVA_FRAMEWORK_H
#define BEVA_FRAMEWORK_H

#include <vector>
#include <string>
#include "ActiveNode.h"
#include "Trie.h"

using namespace std;

class Framework {
public:
    Trie* trie;
    vector<string> records;
    vector<string> queries;
    vector<ActiveNode*> activeNodes;

    Framework();

    void index();
    void process(string, int);
    void output();

    virtual ~Framework();
};


#endif //BEVA_FRAMEWORK_H
