//
// Created by victor on 08/04/2022.
//

#include <vector>
#include <unordered_set>
#include "TopKNode.h"

#ifndef BEVA_TOPKHEAP_H
#define BEVA_TOPKHEAP_H

// this is an structure which implements the
// operator overloading


class TopKHeap {
public:
//    std::priority_queue<TopKNode, std::vector<TopKNode>, compare> heap;
    std::vector<TopKNode> heap;

    TopKHeap();
    ~TopKHeap();

    void insertNode(TopKNode& node);
    std::vector<char *> outputSuggestions();
    bool contains(TopKNode& node);
};

#endif //BEVA_TOPKHEAP_H
