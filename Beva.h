//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "EditVectorAutomaton.h"
#include "Trie.h"

class Beva {
public:
    Trie* trie;
    EditVectorAutomaton* editVectorAutomaton;
    int editDistanceThreshold;

    Beva(Trie*, int);
    ~Beva();

    vector<ActiveNode*> process(string, vector<ActiveNode*>);
};


#endif //BEVA_BEVA_H
