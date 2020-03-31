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

    for(auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::reset(Trie* trie) {
    this->trie = trie;
    for(auto & bitmap : this->bitmaps) bitmap = this->bitmapZero;
}

vector<ActiveNode*> Beva::process(string& query) {
    this->updateBitmap(query);

    if (query.length() == 1) {
        string empty;
        this->currentActiveNodes.push_back(
                new ActiveNode(this->trie->root, this->editVectorAutomata->initialState,empty)
        );
    } else if (query.length() > this->editDistanceThreshold) {
        vector<ActiveNode*> activeNodes;

        for (ActiveNode* oldActiveNode : this->currentActiveNodes) {
            vector<ActiveNode*> newActiveNodes = findActiveNodes(query, oldActiveNode);
            activeNodes.insert(activeNodes.end(), newActiveNodes.begin(), newActiveNodes.end());
        }
        this->currentActiveNodes = activeNodes;
    }
    return this->currentActiveNodes;
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

unsigned Beva::buildBitmap(string& query, string& data) {
    char c = data[(int) data.length() - 1];

    int k = (int) query.length() - (int) data.length();

    return utils::leftShiftBitInDecimal(this->bitmaps[c], this->editDistanceThreshold - k, this->bitmapSize);
}

State* Beva::getNewState(string& query, string& data, State* state) {
    unsigned bitmap = this->buildBitmap(query, data);

    State* newState = state->transitions[bitmap];
    if (newState == nullptr) newState = state;
    return newState;
}

vector<ActiveNode*> Beva::findActiveNodes(string& query, ActiveNode* oldActiveNode) {
    vector<ActiveNode*> activeNodes;

    for (auto &i : oldActiveNode->node->children) {
      string temp = oldActiveNode->data + i->getValue();
        int k = (int) query.length() - (int) temp.length();

        State* newState = this->getNewState(query, temp, oldActiveNode->state);

        if (newState->isFinal) continue;

        if (newState->getEditDistance(k) <= this->editDistanceThreshold) {
            activeNodes.push_back(new ActiveNode(i, newState, temp));
        } else {
            vector<ActiveNode*> newActiveNodes = findActiveNodes(query, new ActiveNode(i, newState, temp));
            activeNodes.insert(activeNodes.end(), newActiveNodes.begin(), newActiveNodes.end());
        }
    }

    return activeNodes;
}
