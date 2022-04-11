//
// Created by victor on 08/04/2022.
//

#include "../header/TopKHeap.h"
#include "../header/Trie.h"
#include <string>

TopKHeap::TopKHeap() {}

void TopKHeap::insertNode(TopKNode node) {
    if (this->heap.size() < 10 || node.getMaxScore() > this->heap.top().maxScore) {
        this->heap.push(node);
        if (this->heap.size() > 10) {
            this->heap.pop();
        }
    }
}

std::vector<char *> TopKHeap::outputSuggestions() {
    std::vector<char *> outputs;
    int i = 0;
    while(!this->heap.empty()) {
        i++;
        unsigned recordId = this->heap.top().recordId;
//        double maxScore = this->heap.top().maxScore;
//        cout << "\tsuggestion " << i << " '" << records[recordId].c_str() << "' score= " << maxScore << endl;
        outputs.push_back(records[recordId].c_str());
        this->heap.pop();
    }

    return outputs;
}