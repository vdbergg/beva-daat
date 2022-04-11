//
// Created by victor on 08/04/2022.
//

#include <queue>
#include <vector>
#include "TopKNode.h"

#ifndef BEVA_TOPKHEAP_H
#define BEVA_TOPKHEAP_H

// this is an structure which implements the
// operator overloading
struct compare {
    bool operator()(TopKNode p1, TopKNode p2)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
        return p1.getMaxScore() < p2.getMaxScore();
    }
};

class TopKHeap {
public:
    std::priority_queue<TopKNode, std::vector<TopKNode>, compare> heap;

    TopKHeap();
//    ~TopKHeap();

    void insertNode(TopKNode node);
    std::vector<char *> outputSuggestions();
};

#endif //BEVA_TOPKHEAP_H
