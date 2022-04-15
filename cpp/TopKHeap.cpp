//
// Created by victor on 08/04/2022.
//

#include "../header/TopKHeap.h"
#include "../header/Trie.h"
#include <string>
#include <algorithm>

TopKHeap::TopKHeap() {
//    this->heap.emplace_back(5, 5.5);
}

TopKHeap::~TopKHeap() {
    this->heap.clear();
}

struct compare {
    bool operator()(TopKNode& p1, TopKNode& p2)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
//        return true;
        return p1.getMaxScore() < p2.getMaxScore();
    }
};

bool TopKHeap::contains(TopKNode& node) {
    for (TopKNode& nodeInHeap: this->heap) {
        if (nodeInHeap.recordId == node.recordId) {
            return true;
        }
    }
    return false;
}

void TopKHeap::insertNode(TopKNode& node) {
//    cout << "heap size: " << this->heap.size() << endl;
    if (!contains(node)) {
        if (this->heap.size() < 10 || node.getMaxScore() > this->heap.front().maxScore) {
            this->heap.push_back(node);
            std::push_heap(this->heap.begin(), this->heap.end(), compare());

            if (this->heap.size() > 10) {
                std::pop_heap(this->heap.begin(), this->heap.end(), compare());
                this->heap.pop_back();
            }
        }
    }

//    cout << "faz algo" << endl;
}

std::vector<char *> TopKHeap::outputSuggestions() {
    std::vector<char *> outputs;
    std::vector<TopKNode> tmp;
    int i = 0;

    for(TopKNode& node : this->heap) {
        tmp.push_back(node);
    }

    while(!tmp.empty()) {
        i++;
        unsigned recordId = tmp.front().recordId;
        double maxScore = tmp.front().maxScore;
//        cout << "\tsuggestion " << i << " '" << records[recordId].c_str() << "' score= " << maxScore << endl;
        outputs.push_back(records[recordId].c_str());
        std::pop_heap(tmp.begin(), tmp.end(), compare());
        tmp.pop_back();
    }
    return outputs;
}