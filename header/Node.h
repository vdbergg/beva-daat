//
// Created by berg on 30/03/2020.
//

#ifndef BEVA_NODE_H
#define BEVA_NODE_H


#include <unordered_map>
#include <iostream>
#include <vector>
#include "ShortVector.h"
#include <vector>
#include "EditVector.h"
#include "State.h"

#define CHAR_SIZE 128

using namespace std;

class Node {
public:
    ShortVector<unsigned> children;
    unsigned beginRange;
    unsigned endRange;

    Node(char value) {
        this->children.init();
        this->setValue(value);
        this->setIsEndOfWord(false);
    }

    Node() {
        this->children.init();
        this->setIsEndOfWord(false);
    }

    inline void setValue(char value) { this->children.value = value; }
    inline char getValue() { return this->children.value; }
    inline void setIsEndOfWord(bool value) { this->children.isEndOfWord = value; }
    inline bool getIsEndOfWord() { return this->children.isEndOfWord; }
    inline unsigned getBeginRange() { return this->beginRange; }
    inline unsigned getEndRange() { return this->endRange;}
    inline void setBeginRange(unsigned value) { this->beginRange = value; }
    inline void setEndRange(unsigned value) { this->endRange = value; }
};

#endif //BEVA_NODE_H
