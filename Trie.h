//
// Created by berg on 13/01/19.
//

#include <string>
#include "Node.h"
#include "ActiveNode.h"
#include <vector>

#ifndef BEVA_TRIE_H
#define BEVA_TRIE_H

using namespace std;

class Trie {
public:
    Node* root;
    vector<ActiveNode*> currentActiveNodes;

    Trie();

    void insert(string, int);
    vector<ActiveNode*> autocomplete(string, int);
    void setEditDistance(string, string, int, int, Node*);
};

#endif //BEVA_TRIE_H
