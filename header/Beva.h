//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "EditVectorAutomata.h"
#include "Trie.h"
#include "ActiveNode.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    EditVectorAutomata* editVectorAutomata;
    int editDistanceThreshold;
    vector<ActiveNode*> currentActiveNodes;
    int bitmapSize;
    unsigned bitmaps[CHAR_SIZE];
    unsigned bitmapZero;
    unsigned bitmapOne;
    unsigned bitmapLast;

    Beva(Trie*, int);
    ~Beva();

    vector<ActiveNode*> process(string&);
    vector<ActiveNode*> findActiveNodes(string&, ActiveNode*);
    State* getNewState(string&, string&, State*);
    unsigned buildBitmap(string&, string&);
    void updateBitmap(string&);
    void reset(Trie*);
};


#endif //BEVA_BEVA_H
