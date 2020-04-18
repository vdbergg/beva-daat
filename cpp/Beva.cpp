//
// Created by berg on 14/02/19.
//

#include <iostream>
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

void Beva::process(string& query) {
    this->updateBitmap(query);

    if (query.length() == 1) {
        this->currentActiveNodes.push_back(ActiveNode(this->trie->root, this->editVectorAutomata->initialState, 0));
        this->experiment->incrementNumberOfActiveNodes(query.length());
    } else if (query.length() > this->editDistanceThreshold) {
        vector<ActiveNode> activeNodes;

        for (ActiveNode oldActiveNode : this->currentActiveNodes) {
            this->findActiveNodes(query.length(), oldActiveNode,activeNodes);
        }
        swap(this->currentActiveNodes,activeNodes);
    }
}

void Beva::updateBitmap(string& query) { // query is equivalent to Q' with the last character c
    char c = query[(int) query.length() - 1];

    for (auto &bitmap : this->bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }

    if (this->bitmaps[c] != this->bitmapZero) {
        if (query.find(c) < query.length() - 1) { // Are two characters in the string window
            // This is an bug in the BEVA algorithm
            // solution: Set the last bit from the right to 1.
            this->bitmaps[c] = utils::setKthBitFromDecimal(this->bitmaps[c], 0, this->bitmapSize);
        }
    } else {
        this->bitmaps[c] =  this->bitmapOne;
    }
}

void Beva::findActiveNodes(unsigned queryLength, ActiveNode &oldActiveNode, vector<ActiveNode> &activeNodes) {
    ShortVector<unsigned>& children = this->trie->getNode(oldActiveNode.node).children;
    unsigned tempSize = oldActiveNode.level + 1;

    for (unsigned child : children) {
        this->experiment->incrementNumberOfIterationInChildren(queryLength);

        State* newState = this->getNewState(queryLength, oldActiveNode.state, tempSize, this->trie->getNode(child).getValue());

        if (newState->isFinal) continue;

        if (newState->getEditDistance((int) queryLength - (int) tempSize) <= this->editDistanceThreshold) {
            activeNodes.push_back(ActiveNode(child, newState, tempSize));
            this->experiment->incrementNumberOfActiveNodes(queryLength);
        } else {
            ActiveNode tmp(child, newState, tempSize);
            this->experiment->incrementNumberOfActiveNodes(queryLength);
            this->findActiveNodes(queryLength, tmp, activeNodes);
        }
    }
}
