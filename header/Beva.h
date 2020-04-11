//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "EditVectorAutomata.h"
#include "Trie.h"
#include "ActiveNode.h"
#include "utils.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    EditVectorAutomata* editVectorAutomata;
    int editDistanceThreshold;
    vector<ActiveNode*> currentActiveNodes;// includes active nodes from all processed prefix since last reset
   
    int bitmapSize;
    unsigned bitmaps[CHAR_SIZE];
    unsigned bitmapZero;
    unsigned bitmapOne;

    Beva(Trie*, int);
    ~Beva();

    void process(string&);
    void findActiveNodes(unsigned, ActiveNode*,  vector<ActiveNode*>&);

    inline unsigned buildBitmap(unsigned queryLength, unsigned lastPosition, char c) {
        int k = (int) queryLength - (int) lastPosition;
        return utils::leftShiftBitInDecimal(this->bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
    }

    inline State* getNewState(unsigned queryLength, State* state, unsigned lastPosition, char c) {
        unsigned bitmap = this->buildBitmap(queryLength, lastPosition, c);

        State* newState = state->transitions[bitmap];
        if (newState == nullptr) newState = state;
        return newState;
    }

    void updateBitmap(string&);
    void reset();
};


#endif //BEVA_BEVA_H
