//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"

vector<StaticString> records;

Trie::Trie(Experiment* experiment) {
    this->experiment = experiment;
    this->globalMemory.reserve(records.size() * 4);

    this->root = newNode();
    getNode(this->root).setBeginRange(0);
    getNode(this->root).setEndRange(records.size());
    this->experiment->incrementNumberOfNodes();

    #ifdef BEVA_IS_BUILD_INDEX_BFS_H
        this->lastNodeKnownPerRecord.reserve(records.size());
        for (int recordId = 0; recordId < records.size(); recordId++) {
            this->lastNodeKnownPerRecord[recordId] = this->root;
        }
    #endif
}

void Trie::buildBfsIndex() {
    int maxLevel = records[0].length();

    for (int currentIndexLevel = 0; currentIndexLevel < maxLevel; currentIndexLevel++) {
        for (int recordId = 0; recordId < records.size(); recordId++) {

            if (currentIndexLevel <= records[recordId].length() - 1) {
                unsigned pattern = this->lastNodeKnownPerRecord[recordId];

                if (records[recordId].length() > maxLevel) {
                    maxLevel = records[recordId].length();
                }

                unsigned char ch = records[recordId][currentIndexLevel];
                unsigned node = this->insert((char) ch, recordId, pattern);
                getNode(node).setEndRange(recordId + 1);
                this->lastNodeKnownPerRecord[recordId] = node;

                if (currentIndexLevel == records[recordId].length() - 1) {
                    getNode(node).setIsEndOfWord(true);
                    #ifdef BEVA_IS_COLLECT_TIME_H
                        this->experiment->proportionOfBranchingSize(currentIndexLevel + 1);
                    #endif
                }
            }
        }
    }
}

unsigned Trie::insert(char ch, int recordId, unsigned parent) {
    unsigned child;
    unsigned numChildren = getNode(parent).numChildren;
    if (numChildren) {
        unsigned child = getNode(parent).children + numChildren - 1;
        if (getNode(child).getValue() == ch) return child;
    }
    unsigned node = newNode();
    if (numChildren == 0) getNode(parent).children = node;

    getNode(node).setValue(ch);
    getNode(node).setBeginRange(recordId);
    getNode(parent).numChildren++;
    #ifdef BEVA_IS_COLLECT_TIME_H
    this->experiment->incrementNumberOfNodes();
    #endif

    return node;
}


void Trie::shrinkToFit() {
    this->globalMemory.shrink_to_fit();

    #ifdef BEVA_IS_BUILD_INDEX_BFS_H
        this->lastNodeKnownPerRecord.clear();
        this->lastNodeKnownPerRecord.shrink_to_fit();
    #endif
}
