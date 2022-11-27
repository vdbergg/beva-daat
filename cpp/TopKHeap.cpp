#include "../header/TopKHeap.h"
#include "../header/Trie.h"

TopKHeap::TopKHeap(unsigned capacity) {
    this->capacity = capacity;
}

TopKHeap::~TopKHeap() {

}

bool TopKHeap::contains(TopKNode& node) {
    return this->recordIdsIntoHeap.find(node.getRecordId()) != this->recordIdsIntoHeap.end();
}

bool TopKHeap::contains(unsigned recordId) {
    return this->recordIdsIntoHeap.find(recordId) != this->recordIdsIntoHeap.end();
}

void TopKHeap::insertNode(TopKNode& node) {
    if (!contains(node)) {
        if (this->heap.size() < this->capacity || node.getMaxScore() > this->topMaxScore()) {
//            cout << "Will insert node with score: " << node.maxScore << endl;
//            print();

            this->heap.push(node);

            this->recordIdsIntoHeap.insert(node.getRecordId());

            if (this->heap.size() > this->capacity) {
                this->recordIdsIntoHeap.erase(this->heap.top().recordId);
                this->heap.pop();
            }

//            cout << "Inserted node with score: " << node.maxScore << endl;
//            print();
        }
    }
}

void TopKHeap::print() {
    cout << "top: " << topMaxScore() << endl;
    cout << "items: " << endl;

    priority_queue<TopKNode&, vector<TopKNode>, myComparator> tmp = this->heap;
    while (!tmp.empty()) {
        cout << tmp.top().recordId << " - " << tmp.top().maxScore << endl;
        tmp.pop();
    }
    cout << endl;
}

vector<char *> TopKHeap::outputSuggestions() {
    vector<char *> outputs;
    priority_queue<TopKNode&, vector<TopKNode>, myComparator> tmp = this->heap;

    while (!tmp.empty()) {
        unsigned recordId = tmp.top().recordId;
        outputs.push_back(records[recordId].c_str());
        tmp.pop();
    }

    return outputs;
}

void TopKHeap::outputSuggestions(vector<char *>& outputs) {
    priority_queue<TopKNode&, vector<TopKNode>, myComparator> tmp = this->heap;

    while (!tmp.empty()) {
        unsigned recordId = tmp.top().recordId;
        outputs.push_back(records[recordId].c_str());
        tmp.pop();
    }
}

bool TopKHeap::isFull() {
    return this->heap.size() >= this->capacity;
}

double TopKHeap::topMaxScore() {
    return this->heap.size() > 0 ? this->heap.top().maxScore : 0;
}