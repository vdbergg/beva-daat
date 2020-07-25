//
// Created by berg on 14/02/19.
//

#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, Experiment* experiment, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->trie = trie;
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->bitmapZero = 0;
    this->bitmapOne = 1;
    this->experiment = experiment;

    for (auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::reset() {
    for(auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;

    this->currentActiveNodes.clear(); // Clean the active nodes for next query
    this->currentActiveNodes.shrink_to_fit();
}

void Beva::process(char ch, int prefixQueryLength) {
    this->updateBitmap(ch);

    if (prefixQueryLength == 1) {
        this->currentActiveNodes.emplace_back(this->trie->root, this->editVectorAutomata->initialState, 0);
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        vector<ActiveNode> activeNodes;

        for (ActiveNode oldActiveNode : this->currentActiveNodes) {
            this->findActiveNodes(prefixQueryLength, oldActiveNode,activeNodes);
        }
        swap(this->currentActiveNodes,activeNodes);
    }
}

void Beva::updateBitmap(char ch) { // query is equivalent to Q' with the last character c
    for (auto &bitmap : this->bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }

    this->bitmaps[ch] = this->bitmaps[ch] | 1;
}

void Beva::findActiveNodes(unsigned queryLength, ActiveNode &oldActiveNode, vector<ActiveNode> &activeNodes) {
    ShortVector<unsigned>& children = this->trie->getNode(oldActiveNode.node).children;
    unsigned tempSize = oldActiveNode.level + 1;

    for (unsigned child : children) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        State* newState = this->getNewState(queryLength, oldActiveNode.state, tempSize,
                this->trie->getNode(child).getValue());

        if (newState->isFinal) continue;

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        if (newState->getEditDistance((int) queryLength - (int) tempSize) <= this->editDistanceThreshold) {
            activeNodes.emplace_back(child, newState, tempSize);
        } else {
            ActiveNode tmp(child, newState, tempSize);
            this->findActiveNodes(queryLength, tmp, activeNodes);
        }
    }
}
