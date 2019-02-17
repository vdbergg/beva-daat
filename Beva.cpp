//
// Created by berg on 14/02/19.
//

#include <utility>
#include <iostream>
#include "Beva.h"
#include "C.h"
#include "ActiveNode.h"
#include "utils.h"

using namespace std;

Beva::Beva(Trie *trie, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (2 * this->editDistanceThreshold) + 1;
    this->trie = trie;
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->trie->root->state = this->editVectorAutomata->initialState;
}

Beva::~Beva() {

}

vector<ActiveNode*> Beva::process(string query, vector<ActiveNode*> oldActiveNodes) {
    Node* root = this->trie->root;
    string data = root->value + "";

    if (query.length() >= this->editDistanceThreshold) {
        if (oldActiveNodes.empty()) {
            findActiveNodes(query, data, root);
        } else {
            this->currentActiveNodes.clear();

            for (ActiveNode* oldActiveNode : oldActiveNodes) {
                data = oldActiveNode->data;
                findActiveNodes(query, data, oldActiveNode->node);
            }
        }
    }
    return this->currentActiveNodes;
}

string Beva::buildBitmap(string query, string data) {
    query = " " + query;
    if (query.length() < this->bitmapSize) {
        for (int i = query.length(); i < this->bitmapSize; i++) {
            query = " " + query;
        }
    }

    string bitmap = "";

    for (int i = 0; i < this->bitmapSize; i++) {
        int temp = int (i + (data.length() - 1));
        if (temp > query.length() - 1) {
            bitmap += data[data.length() - 1] == ' ' ? "1" : "0";
        } else {
            bitmap += data[data.length() - 1] == query[temp] ? "1" : "0";
        }
    }

    return bitmap;
}

bool isOutsideActiveNodeZone(string query, string data, int editDistanceThreshold, Node* node) {
    return data.length() >= query.length() ||
            node->isEndOfWord ||
            (node->state->getEditDistance(query, data) - (query.length() - data.length()) > editDistanceThreshold);
}

void Beva::findActiveNodes(string query, string data, Node *node) {
    if (query.length() == 1) {
        string bitmap = this->buildBitmap(query, data + " ");
        node->state = node->state->transitions[bitmap];

        if (!node->state->isFinal) {
            if (node->state->getEditDistance(query, data + " ") <= this->editDistanceThreshold) {
                this->currentActiveNodes.push_back(new ActiveNode(node, data));
            } else {
                if (isOutsideActiveNodeZone(query, data, this->editDistanceThreshold, node)) {
                    findActiveNodes(query, data, node);
                }
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
                if (isOutsideActiveNodeZone(query, data, this->editDistanceThreshold, i)) {
                    findActiveNodes(query, temp, i);
                }
            }
        }
    }
}
