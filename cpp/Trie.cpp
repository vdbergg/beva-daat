//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/utils.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"

Trie::Trie(int datasetSize, Experiment* experiment) {
    this->experiment = experiment;
    this->root = newNode();
    this->experiment->incrementNumberOfNodes();

    getNode(this->root).setBeginRange(0);
    getNode(this->root).setEndRange(datasetSize);
    this->globalMemory.reserve(datasetSize * 5);
}

void Trie::append(const string& rec, const int recordId) {
    unsigned node = this->root;
    int currentIndexLevel = 0;

    for (unsigned char ch : rec) {
	    node = this->insert((char)ch, recordId, node);

        currentIndexLevel++;
	    getNode(node).setEndRange(recordId + 1);
    }
    getNode(node).setIsEndOfWord(true);
    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->proportionOfBranchingSize(currentIndexLevel);
    #endif
}

unsigned Trie::insert(char ch, int recordId, unsigned node) {
    ShortVector<unsigned>::iterator vit = getNode(node).children.begin();

    for (; vit != getNode(node).children.end(); vit++) {
        if (getNode((*vit)).getValue() == ch) break;
    }

    if (vit == getNode(node).children.end()) {
        unsigned newN = newNode();
        getNode(newN).setValue(ch);
        getNode(newN).setBeginRange(recordId);

        getNode(node).children.push_back(newN);
        #ifdef BEVA_IS_COLLECT_TIME_H
            this->experiment->incrementNumberOfNodes();
        #endif
        return newN;
    }

    return *vit;
}

void Trie::shrinkToFit() {
    this->globalMemory.shrink_to_fit();

    for (auto node : this->globalMemory) {
        node.children.shrink_to_fit();
    }
}
