//
// Created by berg on 13/01/19.
//

#include <string>
#include "Node.h"
#include "Experiment.h"

#ifndef BEVA_TRIE_H
#define BEVA_TRIE_H

using namespace std;

class Trie {
public:
    vector<Node> globalMemory;
    Node* root;
    Experiment* experiment;

    Trie(int, Experiment*);

    void append(const string&, const int);
    Node* insert(char ch, int, Node*);
};

#endif //BEVA_TRIE_H
