//
// Created by berg on 14/02/19.
//

#include <utility>
#include "Beva.h"

using namespace std;

Beva::Beva(Trie *trie, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->trie = trie;
    this->editVectorAutomaton = new EditVectorAutomaton(this->editDistanceThreshold);
    this->editVectorAutomaton->buildAutomaton();
}

Beva::~Beva() {

}

vector<ActiveNode*> Beva::process(string query, vector<ActiveNode*> oldActiveNodes) {
    return this->trie->autocomplete(move(query), this->editDistanceThreshold, move(oldActiveNodes));
}
