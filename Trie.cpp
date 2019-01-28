//
// Created by berg on 13/01/19.
//

#include <iostream>
#include "Trie.h"
#include "EditDistance.h"
#include <chrono>

bool isOutsideActiveNodeZone(string query, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    return query.length() >= queryOriginalLength
            || node->isEndOfWord
            || (node->editDistance != nullptr && node->getEditDistance() != 30
            && (node->getEditDistance() - (queryOriginalLength - query.length()) > editDistanceThreshold // fora de alcance
               || node->getEditDistance() + (queryOriginalLength - query.length()) <= editDistanceThreshold)); // já alcançado
}

bool isToPrint(string query, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    return (query.length() == queryOriginalLength || node->isEndOfWord
           || node->getEditDistance() + (queryOriginalLength - query.length()) <= editDistanceThreshold) // já alcançado
           && (node->editDistance != nullptr && node->getEditDistance() != 30)
           && node->getEditDistance() <= editDistanceThreshold;
}

Trie::Trie() {
    this->root = new Node();
}

void Trie::insert(string key) {
    Node* root = this->root;

    for (char i : key) {
        if (root->children[i] == nullptr) {
            root->children[i] = new Node(i, root);
        }
        root = root->children[i];
    }

    root->isEndOfWord = true;
}

Node* search(string key, Node* root) {
    for (char i : key) {
        root = root->children[i];
        if (root == nullptr) return nullptr;
    }

    return root;
}

void printSuggestions(string queryOriginal, string query, Node *node) {
    for (auto &i : node->children) {
        if (i == nullptr) continue;
        printSuggestions(queryOriginal, query + i->value, i);
    }
    if (node->isEndOfWord) cout << query + "\n";
}

void searchSuggestions(string queryOriginal, string query, int editDistanceThreshold, Node *node) {
    if ((query.length() == queryOriginal.length() || node->isEndOfWord)
        && node->editDistance != nullptr && node->getEditDistance() <= editDistanceThreshold) {
        printSuggestions(queryOriginal, query, node);
    } else {
        if (isOutsideActiveNodeZone(query, (int) queryOriginal.length(), editDistanceThreshold, node)) {
            return;
        }

        for (auto &i : node->children) {
            if (i == nullptr) continue;
            searchSuggestions(queryOriginal, query + i->value, editDistanceThreshold, i);
        }
    }
}

void setEditDistance(string queryOriginal, string query, int queryOriginalLength, int editDistanceThreshold, Node* node) {
    node->calculateEditDistance(queryOriginal, query);

    if (isToPrint(query, queryOriginalLength, editDistanceThreshold, node)) {
        printSuggestions(queryOriginal, query, node);
        return;
    }

    if (isOutsideActiveNodeZone(query, queryOriginalLength, editDistanceThreshold, node)) {
        return;
    }

    for (auto &i : node->children) {
        if (i == nullptr) continue;
        setEditDistance(queryOriginal, query + i->value, queryOriginalLength, editDistanceThreshold, i);
    }
}

void Trie::autocomplete(string query, int editDistanceThreshold) {
    string empty = "" + this->root->value;
    setEditDistance(query, empty, (int) query.length(), editDistanceThreshold, this->root);
}
