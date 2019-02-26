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
    this->trie->root->state = this->editVectorAutomata->initialState;
    this->bitmapZero = utils::convertDecimalToBinaryString(0, this->bitmapSize);
    this->bitmapOne = utils::convertDecimalToBinaryString(1, this->bitmapSize);

    for (int i = 0; i < this->bitmapSize; i++) {
        this->bitmapLast += "1";
    }
    this->bitmaps['\0'] = this->bitmapLast;
}

Beva::~Beva() {

}

void Beva::reset(Trie* trie) {
    this->trie = trie;
    this->trie->root->state = this->editVectorAutomata->initialState;
    this->bitmaps->clear();
    this->bitmaps['\0'] = this->bitmapLast;
}

vector<ActiveNode*> Beva::process(string query, vector<ActiveNode*> oldActiveNodes) {
    for (char &c : query) {
        if ((int) c == -61) continue;
        else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
            c = utils::convertSpecialCharToSimpleChar(c);
        }
    }

    this->currentActiveNodes.clear();

    this->updateBitmap(query);

    Node* root = this->trie->root;
    string data = root->value + "";

    if (query.length() >= this->editDistanceThreshold) {
        if (oldActiveNodes.empty()) {
            findActiveNodes(query, data, root);
        } else {
            for (ActiveNode* oldActiveNode : oldActiveNodes) {
                data = oldActiveNode->data;
                findActiveNodes(query, data, oldActiveNode->node);
            }
        }
    }
    return this->currentActiveNodes;
}

void Beva::updateBitmap(string query) { // query is equivalent to Q' with the last character c
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

string Beva::buildBitmap(string query, string data) {
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

void Beva::findActiveNodes(string query, string data, Node *node) {
    if (query.length() == 1) {
        string bitmap = this->buildBitmap(query, data);
        node->state = node->state->transitions[bitmap];

        if (!node->state->isFinal) {
            if (node->state->getEditDistance(query, data) <= this->editDistanceThreshold) {
                this->currentActiveNodes.push_back(new ActiveNode(node, data));
            } else {
                findActiveNodes(query, data, node);
            }
        }
        return;
    }

    for (auto &i : node->children) {
        if (i == nullptr) continue;

        string temp = data + i->value;

        string bitmap = this->buildBitmap(query, temp);
        i->state = node->state->transitions[bitmap];

        if (!i->state->isFinal) {
            if (i->state->getEditDistance(query, temp) <= this->editDistanceThreshold) {
                this->currentActiveNodes.push_back(new ActiveNode(i, temp));
            } else {
                findActiveNodes(query, temp, i);
            }
        }
    }
}
