//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"

Trie::Trie(vector<string>& records, Experiment* experiment) {
    this->experiment = experiment;
    this->records = records;
    this->globalMemory.reserve(this->records.size() * 5);

    this->root = newNode();
    getNode(this->root).setBeginRange(0);
    getNode(this->root).setEndRange(this->records.size());
    this->experiment->incrementNumberOfNodes();

    #ifdef BEVA_IS_BUILD_INDEX_BFS_H
        this->lastNodeKnownPerRecord.reserve(this->records.size());
        for (int recordId = 0; recordId < this->records.size(); recordId++) {
            this->lastNodeKnownPerRecord[recordId] = this->root;
        }
    #endif
}

void Trie::buildBfsIndex() {
    int maxLevel = this->records[0].length();

    for (int currentIndexLevel = 0; currentIndexLevel < maxLevel; currentIndexLevel++) {
        for (int recordId = 0; recordId < this->records.size(); recordId++) {

            if (currentIndexLevel <= this->records[recordId].length() - 1) {
                unsigned pattern = this->lastNodeKnownPerRecord[recordId];

                if (this->records[recordId].length() > maxLevel) {
                    maxLevel = this->records[recordId].length();
                }

                unsigned char ch = this->records[recordId][currentIndexLevel];
                unsigned node = this->insert((char) ch, recordId, pattern);
                getNode(node).setEndRange(recordId + 1);
                this->lastNodeKnownPerRecord[recordId] = node;

                if (currentIndexLevel == this->records[recordId].length() - 1) {
                    getNode(node).setIsEndOfWord(true);
                    #ifdef BEVA_IS_COLLECT_TIME_H
                        this->experiment->proportionOfBranchingSize(currentIndexLevel + 1);
                    #endif
                }
            }
        }
    }
}

void Trie::buildDfsIndex() {
    for (int recordId = 0; recordId < this->records.size(); recordId++) {
        unsigned node = this->root;
        int currentIndexLevel = 0;

        for (unsigned char ch : this->records[recordId]) {
            node = this->insert((char)ch, recordId, node);

            currentIndexLevel++;
            getNode(node).setEndRange(recordId + 1);
        }
        getNode(node).setIsEndOfWord(true);
        #ifdef BEVA_IS_COLLECT_TIME_H
                this->experiment->proportionOfBranchingSize(currentIndexLevel);
        #endif
    }
}

unsigned Trie::insert(char ch, int recordId, unsigned pattern) {
    ShortVector<unsigned>::iterator vit = getNode(pattern).children.begin();

    for (; vit != getNode(pattern).children.end(); vit++) {
        if (getNode((*vit)).getValue() == ch) break;
    }

    if (vit == getNode(pattern).children.end()) {
        unsigned node = newNode();
        getNode(node).setValue(ch);
        getNode(node).setBeginRange(recordId);

        getNode(pattern).children.push_back(node);
        #ifdef BEVA_IS_COLLECT_TIME_H
            this->experiment->incrementNumberOfNodes();
        #endif
        return node;
    }

    return *vit;
}

void Trie::shrinkToFit() {
    this->globalMemory.shrink_to_fit();

    #ifdef BEVA_IS_BUILD_INDEX_BFS_H
        this->lastNodeKnownPerRecord.clear();
        this->lastNodeKnownPerRecord.shrink_to_fit();
    #endif

    for (auto node : this->globalMemory) {
        node.children.shrink_to_fit();
    }
}
