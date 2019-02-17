//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "EditVectorAutomata.h"
#include "Trie.h"
#include "ActiveNode.h"

class Beva {
public:
    Trie* trie;
    EditVectorAutomata* editVectorAutomata;
    int editDistanceThreshold;
    vector<ActiveNode*> currentActiveNodes;
    int bitmapSize;

    Beva(Trie*, int);
    ~Beva();

    vector<ActiveNode*> process(string, vector<ActiveNode*>);
    void findActiveNodes(string query, string data, Node *node);
    string buildBitmap(string query, string data);
};


#endif //BEVA_BEVA_H
