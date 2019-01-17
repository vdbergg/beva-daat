//
// Created by berg on 13/01/19.
//

#include <iostream>
#include "Trie.h"
#include "EditDistance.h"

Trie::Trie() {
    this->root = new Node();
}

void Trie::insert(string key) {
    Node* root = this->root;

    for (char i : key) {
        if (root->children[i] == nullptr) {
            root->children[i] = new Node(i);
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

void printSuggestions(string queryOriginal, string query, int tau, Node *node) {
    for (auto &i : node->children) {
        if (i == nullptr) continue;
        printSuggestions(queryOriginal, query + i->value, tau, i);
    }
    if (node->isEndOfWord) cout << query + "\n";
}

void searchSuggestions(string queryOriginal, string query, int tau, Node *node) {
    if ((query.length() == queryOriginal.length() || node->isEndOfWord) && node->editDistance <= tau) {
        printSuggestions(queryOriginal, query, tau, node);
    } else {
        for (auto &i : node->children) {
            if (i == nullptr) continue;
            searchSuggestions(queryOriginal, query + i->value, tau, i);
        }
    }
}

void setEditDistance(string queryOriginal, string query, int tau, Node* node) {
    node->editDistance = editDistance(queryOriginal, query);

    if ((query.length() == queryOriginal.length() || node->isEndOfWord) && node->editDistance <= tau) {
        return;
    }

    for (auto &i : node->children) {
        if (i == nullptr) continue;
        setEditDistance(queryOriginal, query + i->value, tau, i);
    }
}

void Trie::autocomplete(string query, int tau) {
    string empty = "" + this->root->value;
    cout << "setEditDistance() \n";
    setEditDistance(query, empty, tau, this->root);
    cout << "searchSuggestions() \n";
    searchSuggestions(query, empty, tau, this->root);
}
