//
// Created by berg on 13/01/19.
//

#include <string>
#include "Node.h"

#ifndef BEVA_TRIE_H
#define BEVA_TRIE_H

using namespace std;

class Trie {
public:
    Node* root;

    Trie();

    void append(const char*, const int);
    Node* insert(char ch, Node*);
};

#endif //BEVA_TRIE_H
