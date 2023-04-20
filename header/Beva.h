//
// Created by berg on 14/02/19.
//

#ifndef BEVA_BEVA_H
#define BEVA_BEVA_H


#include "EditVectorAutomata.h"
#include "Trie.h"
#include "ActiveNode.h"
#include "utils.h"
#include "TopKHeap.h"

#define CHAR_SIZE 128

class Beva {
public:
    Trie* trie;
    EditVectorAutomata* editVectorAutomata;
    Experiment *experiment;
    int editDistanceThreshold;

    int bitmapSize;
    unsigned bitmapZero;
    long long *preCalculatedExponentiation;

    Beva(Trie*, Experiment*, int, long long *preCalculatedExponentiation);
    ~Beva();

    void processNoErrors(char ch,
                         int prefixQueryLength,
                         vector<ActiveNode>& oldNoErrorActiveNodes,
                         vector<ActiveNode>& currentNoErrorActiveNodes);

    inline unsigned buildBitmap(unsigned queryLength, unsigned lastPosition, char c, unsigned (&bitmaps)[CHAR_SIZE]) {
        int k = (int) queryLength - (int) lastPosition;
        return utils::leftShiftBitInDecimal(bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
    }
    
    inline State* getNewState(unsigned queryLength, State* state, unsigned lastPosition, char c,
            unsigned (&bitmaps)[CHAR_SIZE]) {
        unsigned bitmap = this->buildBitmap(queryLength, lastPosition, c, bitmaps);

        State* newState = state->transitions[bitmap];
        if (newState == nullptr) newState = state;
        return newState;
    }

    void updateBitmap(char, unsigned (&bitmaps)[CHAR_SIZE]);

    void process(char ch,
                 int prefixQueryLength,
                 vector<ActiveNode>& oldActiveNodes,
                 vector<ActiveNode>& currentActiveNodes,
                 unsigned (&bitmaps)[CHAR_SIZE]);

    void processWithPruningV2(char ch,
                            int prefixQueryLength,
                            vector<ActiveNode>& oldActiveNodes,
                            vector<ActiveNode>& currentActiveNodes,
                            unsigned (&bitmaps)[CHAR_SIZE],
                            TopKHeap& topKHeap);

    void findActiveNodes(unsigned queryLength,
                         ActiveNode &oldActiveNode,
                         vector<ActiveNode> &activeNodes,
                         unsigned (&bitmaps)[CHAR_SIZE]);

    void findActiveNodesWithPruningV2(unsigned queryLength,
                                      ActiveNode &oldActiveNode,
                                      vector<ActiveNode> &activeNodes,
                                      unsigned (&bitmaps)[CHAR_SIZE],
                                      TopKHeap& topKHeap);
};


#endif //BEVA_BEVA_H
