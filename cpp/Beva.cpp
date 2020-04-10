//
// Created by berg on 14/02/19.
//

#include <iostream>
#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->trie = trie;
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->bitmapZero = 0;
    this->bitmapOne = 1;

    for (auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::reset() {
    for(auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;

    for (auto* activeNode : this->currentActiveNodes) delete activeNode;
    this->currentActiveNodes.clear(); // Clean the active nodes for next query
    this->currentActiveNodes.shrink_to_fit();
}

void Beva::process(string& query) {
    this->updateBitmap(query);

    if (query.length() == 1) {
        string empty;
        this->currentActiveNodes.push_back(
                new ActiveNode(this->trie->root, this->editVectorAutomata->initialState,empty)
                );
    } else if (query.length() > this->editDistanceThreshold) {
        vector<ActiveNode*> activeNodes;

        for (ActiveNode* oldActiveNode : this->currentActiveNodes) {
            this->findActiveNodes(query.length(), oldActiveNode,activeNodes);
            delete oldActiveNode;
        }
        this->currentActiveNodes.clear();
        this->currentActiveNodes = activeNodes;
    }
}

void Beva::updateBitmap(string& query) { // query is equivalent to Q' with the last character c
    char c = query[(int) query.length() - 1];

    for (auto &bitmap : this->bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }

    string temp = query;
    temp.erase(query.length() - 1);
    if (this->bitmaps[c] != this->bitmapZero && temp.find(c) != string::npos) { // Are two characters in the string window
        // This is an bug in the BEVA algorithm
        // solution: Set the last bit from the right to 1.
        this->bitmaps[c] = utils::setKthBitFromDecimal(this->bitmaps[c], 0, this->bitmapSize);
    } else {
        this->bitmaps[c] = this->bitmaps[c] == this->bitmapZero ? this->bitmapOne : this->bitmaps[c];
    }
}

unsigned Beva::buildBitmap(unsigned queryLength, string& data) {
    char c = data[(int) data.length() - 1];

    int k = (int) queryLength - (int) data.length();

    return utils::leftShiftBitInDecimal(this->bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
}

State* Beva::getNewState(unsigned queryLength, string& data, State* state) {
    unsigned bitmap = this->buildBitmap(queryLength, data);

    State* newState = state->transitions[bitmap];
    if (newState == nullptr) newState = state;
    return newState;
}

void Beva::findActiveNodes(unsigned queryLength, ActiveNode* oldActiveNode, vector<ActiveNode*> &activeNodes) {
    for (auto &i : this->trie->getNode(oldActiveNode->node).children) {
        string temp = oldActiveNode->data +  this->trie->getNode(i).getValue();
        int k = (int) queryLength - (int) temp.length();

        State* newState = this->getNewState(queryLength, temp, oldActiveNode->state);

        if (newState->isFinal) continue;

        if (newState->getEditDistance(k) <= this->editDistanceThreshold) {
            activeNodes.push_back(new ActiveNode(i, newState, temp));
        } else {
            ActiveNode tmp(i, newState, temp);
            this->findActiveNodes(queryLength, &tmp, activeNodes);
        }
    }
}
