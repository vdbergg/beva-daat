//
// Created by berg on 14/02/19.
//

#include <utility>
#include <iostream>
#include <bitset>
#include "../header/Beva.h"
#include "../header/C.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (2 * this->editDistanceThreshold) + 1;
    this->trie = trie;
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->bitmapZero = utils::convertDecimalToBinaryString(0, this->bitmapSize);
    this->bitmapOne = utils::convertDecimalToBinaryString(1, this->bitmapSize);

    for (int i = 0; i < this->bitmapSize; i++) {
        this->bitmapLast += "1";
    }
    this->bitmaps['\0'] = this->bitmapLast;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::reset(Trie* trie) {
    this->trie = trie;
    this->bitmaps->clear();
    this->bitmaps['\0'] = this->bitmapLast;
}

vector<ActiveNode*> Beva::process(string& query, vector<ActiveNode*>& oldActiveNodes) {
    if (!this->currentActiveNodes.empty()) this->currentActiveNodes.clear();

    this->updateBitmap(query);

    if (query.length() <= this->editDistanceThreshold) return oldActiveNodes;

    for (ActiveNode* oldActiveNode : oldActiveNodes) {
        findActiveNodes(query, oldActiveNode);
    }
    return this->currentActiveNodes;
}

void Beva::updateBitmap(string& query) { // query is equivalent to Q' with the last character c
    char c = query[(int) query.length() - 1];

    for (auto &bitmap : this->bitmaps) {
        if (bitmap.empty()) continue;

        string newBitmap = utils::shiftBitInBinaryString(bitmap, 1, this->bitmapSize);

        if (newBitmap != this->bitmapZero) {
            bitmap = newBitmap;
        } else {
            bitmap = "";
        }
    }
    string temp = query;
    temp.erase(query.length() - 1);
    if (temp.find(c) != string::npos && !this->bitmaps[c].empty()) { // Are two characters in the string window
        this->bitmaps[c].replace(this->bitmaps[c].length() - 1, this->bitmaps[c].length(), "1");
    } else {
        this->bitmaps[c] = this->bitmaps[c].empty() ? this->bitmapOne : this->bitmaps[c];
    }
}

string Beva::buildBitmap(string& query, string& data) {
    char c = data[(int) data.length() - 1];
    string bitmap = this->bitmaps[c];

    if (bitmap.empty()) {
        bitmap = this->bitmapZero;
    } else {
        int k = (int) query.length() - (int) data.length();

        bitmap = utils::shiftBitInBinaryString(bitmap, this->editDistanceThreshold - k, this->bitmapSize);
    }

    return bitmap;
}

State* Beva::getNewState(string& query, string& data, State* state) {
    string bitmap = this->buildBitmap(query, data);

    State* newState = state->transitions[bitmap];
    if (newState == nullptr) newState = state;
    return newState;
}

void Beva::findActiveNodes(string& query, ActiveNode* oldActiveNode) {
    string data = oldActiveNode->data;
    Node* node = oldActiveNode->node;
    State* state = oldActiveNode->state;

    for (auto &i : node->children) {
        string temp = data + i->value;
        State* newState = this->getNewState(query, temp, state);

        if (newState->isFinal) continue;

        ActiveNode* activeNode = new ActiveNode(i, newState, temp);
        if (temp.length() >= query.length() &&
            newState->getEditDistance(query, temp) <= this->editDistanceThreshold) {
            this->currentActiveNodes.push_back(activeNode);
        } else {
            findActiveNodes(query, activeNode);
        }
    }
}
