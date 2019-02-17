//
// Created by berg on 13/01/19.
//

#include "Trie.h"
#include "C.h"

Trie::Trie() {
    this->root = new Node();
}

void Trie::insert(string record, int recordId) {
    Node* root = this->root;

    for (char i : record) {
        if (root->children[i] == nullptr) {
            root->children[i] = new Node(i);
        }
        root->children[i]->recordsId.push_back(recordId);
        root = root->children[i];
    }

    root->isEndOfWord = true;
}
