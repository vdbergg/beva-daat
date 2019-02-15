//
// Created by berg on 13/01/19.
//

#include <iostream>
#include "Trie.h"
#include "C.h"
#include <chrono>

bool isOutsideActiveNodeZone(string query, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    return query.length() >= queryOriginalLength
           || node->isEndOfWord
           || (node->editVector != nullptr && node->getEditDistance() != C::MARKER
               && (node->getEditDistance() - (queryOriginalLength - query.length()) > editDistanceThreshold // fora de alcance
                   || node->getEditDistance() + (queryOriginalLength - query.length()) <= editDistanceThreshold)); // já alcançado
}

bool isToPrint(string query, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    return (query.length() == queryOriginalLength || node->isEndOfWord
            || node->getEditDistance() + (queryOriginalLength - query.length()) <= editDistanceThreshold) // já alcançado
           && (node->editVector != nullptr && node->getEditDistance() != C::MARKER)
           && node->getEditDistance() <= editDistanceThreshold;
}

Trie::Trie() {
    this->root = new Node();
}

void Trie::insert(string record, int recordId) {
    Node* root = this->root;

    for (char i : record) {
        if (root->children[i] == nullptr) {
            root->children[i] = new Node(i, root);
        }
        root->children[i]->recordsId.push_back(recordId);
        root = root->children[i];
    }

    root->isEndOfWord = true;
}

void Trie::setEditDistance(string query, string data, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    node->calculateEditDistance(query, data, editDistanceThreshold);

    if (isToPrint(data, queryOriginalLength, editDistanceThreshold, node)) {
        this->currentActiveNodes.push_back(new ActiveNode(node, data));
        return;
    }

    if (isOutsideActiveNodeZone(data, queryOriginalLength, editDistanceThreshold, node)) {
        return;
    }

    for (auto &i : node->children) {
        if (i == nullptr) continue;
        setEditDistance(query, data + i->value, queryOriginalLength, editDistanceThreshold, i);
    }
}

vector<ActiveNode*> Trie::autocomplete(string query, int editDistanceThreshold, vector<ActiveNode*> activeNodes) {
    string data;

    if (activeNodes.empty()) {
        data = "" + this->root->value;
        setEditDistance(query, data, (int) query.length(), editDistanceThreshold, this->root);
    } else {
        this->currentActiveNodes.clear();

        for (ActiveNode* activeNode : activeNodes) {
            data = activeNode->data;
            setEditDistance(query, data, (int) query.length(), editDistanceThreshold, activeNode->node);
        }
    }
    return this->currentActiveNodes;
}
