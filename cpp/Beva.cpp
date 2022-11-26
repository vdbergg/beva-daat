//
// Created by berg on 14/02/19.
//

#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, Experiment* experiment, int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->trie = trie;
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->bitmapZero = 0;
    this->bitmapOne = 1;
    this->experiment = experiment;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::process(char ch,
                   int prefixQueryLength,
                   vector<ActiveNode>& oldActiveNodes,
                   vector<ActiveNode>& currentActiveNodes,
                   unsigned (&bitmaps)[CHAR_SIZE]
                   ){

    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        currentActiveNodes.emplace_back(this->trie->root, this->editVectorAutomata->initialState, 0);
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        for (ActiveNode oldActiveNode : oldActiveNodes) {
            this->findActiveNodes(prefixQueryLength, oldActiveNode, currentActiveNodes, bitmaps);
        }
    } else {
        swap(currentActiveNodes, oldActiveNodes);
    }
}

void Beva::updateBitmap(char ch, unsigned (&bitmaps)[CHAR_SIZE]) { // query is equivalent to Q' with the last character c
    for (auto &bitmap : bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }
  
    bitmaps[ch] = bitmaps[ch] | 1;
}

void Beva::findActiveNodes(unsigned queryLength,
                           ActiveNode &oldActiveNode,
                           vector<ActiveNode> &activeNodes,
                           unsigned (&bitmaps)[CHAR_SIZE]) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChilds = child + this->trie->getNode(oldActiveNode.node).numChildren;

    unsigned tempSize = oldActiveNode.level + 1;
    for (; child < endChilds; child++) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        State* newState = this->getNewState(queryLength, oldActiveNode.state, tempSize,
                this->trie->getNode(child).getValue(), bitmaps);

        if (newState->isFinal) continue;


        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        if (newState->getEditDistance((int) queryLength - (int) tempSize) <= this->editDistanceThreshold) {
            activeNodes.emplace_back(child, newState, tempSize);
        } else {
            ActiveNode tmp(child, newState, tempSize);
            this->findActiveNodes(queryLength, tmp, activeNodes, bitmaps);
        }
    }
}

void Beva::processTopK(char ch,
                       int prefixQueryLength,
                       vector<vector<ActiveNode>>& oldActiveNodes,
                       vector<vector<ActiveNode>>& currentActiveNodes,
                       unsigned (&bitmaps)[3][CHAR_SIZE],
                       TopKHeap& heap
){

    this->updateBitmap(ch, bitmaps[this->editDistanceThreshold]);


    if (prefixQueryLength == 1) {
        currentActiveNodes[this->editDistanceThreshold].emplace_back(ActiveNode(this->trie->root, this->editVectorAutomata->initialState, 0));
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
            this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        for (ActiveNode oldActiveNode : oldActiveNodes[this->editDistanceThreshold]) {
            this->findActiveNodesMultiBeva(prefixQueryLength, oldActiveNode, currentActiveNodes, bitmaps, heap);
        }
    } else {
        swap(currentActiveNodes[this->editDistanceThreshold], oldActiveNodes[this->editDistanceThreshold]);
    }
}

void Beva::findActiveNodesMultiBeva(unsigned queryLength,
                                    ActiveNode &oldActiveNode,
                                    vector<vector<ActiveNode>> &activeNodes,
                                    unsigned (&bitmaps)[3][CHAR_SIZE],
                                    TopKHeap& topKHeap) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChilds = child + this->trie->getNode(oldActiveNode.node).numChildren;

    unsigned tempSize = oldActiveNode.level + 1;
//    cout << this->trie->getNode(oldActiveNode.node).isEndOfWord << endl;
//    cout << child << endl;
//    cout << endChilds << endl;
    for (; child < endChilds; child++) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        State* newState = this->getNewState(queryLength, oldActiveNode.state, tempSize,
                                            this->trie->getNode(child).getValue(), bitmaps[this->editDistanceThreshold]);

        if (newState->isFinal) continue;

        double childScore = utils::dynamicScore(this->trie->getNode(child).maxStaticScore,
                                                this->editDistanceThreshold,
                                                queryLength,
                                                2);

        if (topKHeap.heap.size() >= 10 &&
            childScore < topKHeap.heap.front().maxScore) continue;

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        if (newState->getEditDistance((int) queryLength - (int) tempSize) <= this->editDistanceThreshold) {
            activeNodes[this->editDistanceThreshold].emplace_back(child, newState, tempSize);
        } else {
            ActiveNode tmp(child, newState, tempSize);
            this->findActiveNodesMultiBeva(queryLength, tmp, activeNodes, bitmaps, topKHeap);
        }
    }
}
