//
// Created by berg on 13/01/19.
//

#include <iostream>
#include "Trie.h"

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

void getSuggestions(string query, Node* node) {
    for (auto &i : node->children) {
        if (i == nullptr) continue;
        getSuggestions(query + i->value, i);
    }

    if (node->isEndOfWord) cout << query + "\n";
}

void Trie::autocomplete(string query) {
    Node* node = search(query, this->root);
    if (node != nullptr) getSuggestions(query, node);
}
