//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/utils.h"

Trie::Trie() {
    this->root = new Node();
}

void Trie::append(const char *str, const int recordId) {
    Node* node = this->root;
    while (*str) {
        node = this->insert(*str, node);
        node->recordsId.push_back(recordId);
        ++str;
    }
}

Node* Trie::insert(char ch, Node* node) {
    auto vit = node->children.begin();
    for (; vit != node->children.end(); vit++) {
        if ((*vit)->value == ch) break;
    }

    if (vit == node->children.end()) {
        Node* newNode = new Node(ch);
        node->children.push_back(newNode);
        return newNode;
    }
    return *vit;
}
