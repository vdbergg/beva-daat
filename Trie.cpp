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

void getSuggestions(string queryOriginal, string query, int tau, Node* node) {
    for (auto &i : node->children) {
        if (i == nullptr) continue;
        getSuggestions(queryOriginal, query + i->value, tau, i);
    }

    if (node->isEndOfWord && node->editDistance <= tau) {
        cout << query + "\n";
    }
}

void setEditDistance(string queryOriginal, string query, Node* node) {
    node->editDistance = editDistance(queryOriginal, query);

    for (auto &i : node->children) {
        if (i == nullptr) continue;
        setEditDistance(queryOriginal, query + i->value, i);
    }
}

void Trie::autocomplete(string query) {
    string empty = "" + this->root->value;
    setEditDistance(query, empty, this->root);
    int tau = 3;
    getSuggestions(query, empty, tau, this->root);
}
