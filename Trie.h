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

    void insert(string);
    void autocomplete(string, int);
};

#endif //BEVA_TRIE_H
