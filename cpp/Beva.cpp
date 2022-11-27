//
// Created by berg on 14/02/19.
//

#include "../header/Beva.h"
#include "../header/ActiveNode.h"
#include "../header/utils.h"

using namespace std;

Beva::Beva(Trie *trie, Experiment* experiment, int editDistanceThreshold, long long *preCalculatedExponentiation) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->trie = trie;
    this->bitmapSize = (1 << ((2 * this->editDistanceThreshold) + 1)) - 1; // 2^(2tau + 1) - 1
    this->editVectorAutomata = new EditVectorAutomata(this->editDistanceThreshold);
    this->editVectorAutomata->buildAutomaton();
    this->bitmapZero = 0;
    this->experiment = experiment;
    this->preCalculatedExponentiation = preCalculatedExponentiation;
}

Beva::~Beva() {
    delete this->editVectorAutomata;
}

void Beva::processNoErrors(char ch,
                           int prefixQueryLength,
                           vector<ActiveNode>& oldActiveNodes,
                           vector<ActiveNode>& currentActiveNodes) {
    if (prefixQueryLength == 1) {
        oldActiveNodes.emplace_back(ActiveNode(this->trie->root, this->editVectorAutomata->initialState, 0, 0));
    }

    for (ActiveNode oldActiveNode : oldActiveNodes) { // This for loop always will have one or zero item
        unsigned child = this->trie->getNode(oldActiveNode.node).children;
        unsigned endChildren = child + this->trie->getNode(oldActiveNode.node).numChildren;
        unsigned tempSize = oldActiveNode.level + 1;

        for (; child < endChildren; child++) {
            if (ch == this->trie->getNode(child).getValue()) {
                #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfActiveNodes(queryLength);
                #endif
                currentActiveNodes.emplace_back(child, nullptr, tempSize, 0);
                break;
            }
        }
    }
}

void Beva::updateBitmap(char ch, unsigned (&bitmaps)[CHAR_SIZE]) { // query is equivalent to Q' with the last character c
    for (auto &bitmap : bitmaps) {
        bitmap = utils::leftShiftBitInDecimal(bitmap, 1, this->bitmapSize);
    }
  
    bitmaps[ch] = bitmaps[ch] | 1;
}

void Beva::process(char ch,
                   int prefixQueryLength,
                   vector<ActiveNode>& oldActiveNodes,
                   vector<ActiveNode>& currentActiveNodes,
                   unsigned (&bitmaps)[CHAR_SIZE]) {
    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        currentActiveNodes.emplace_back(ActiveNode(this->trie->root, this->editVectorAutomata->initialState, 0, prefixQueryLength));
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

void Beva::processWithPruningV2(char ch,
                                int prefixQueryLength,
                                vector<ActiveNode>& oldActiveNodes,
                                vector<ActiveNode>& currentActiveNodes,
                                unsigned (&bitmaps)[CHAR_SIZE],
                                TopKHeap& heap) {
    this->updateBitmap(ch, bitmaps);

    if (prefixQueryLength == 1) {
        double childScore = utils::dynamicScore(this->trie->getNode(this->trie->root).getMaxStaticScore(),
                                                this->preCalculatedExponentiation[this->editDistanceThreshold]);
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(this->trie->root).getRecordIdFromMaxScore();

        if (heap.isFull() && (childScore < heap.topMaxScore() || heap.contains(recordIdFromActiveNodeScore))) return;

        currentActiveNodes.emplace_back(this->trie->root, this->editVectorAutomata->initialState, 0, prefixQueryLength);

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(query.length());
        #endif
    } else if (prefixQueryLength > this->editDistanceThreshold) {
        for (ActiveNode oldActiveNode : oldActiveNodes) {
            this->findActiveNodesWithPruningV2(prefixQueryLength,
                                               oldActiveNode,
                                               currentActiveNodes,
                                               bitmaps,
                                               heap);
        }
    } else {
        swap(currentActiveNodes, oldActiveNodes);
//        for (ActiveNode& activeNode : currentActiveNodes) { // This code is necessary when the edit distance were obtained in fetching phase
//            activeNode.editDistance++;
//        }
    }
}

void Beva::findActiveNodes(unsigned queryLength,
                           ActiveNode &oldActiveNode,
                           vector<ActiveNode> &activeNodes,
                           unsigned (&bitmaps)[CHAR_SIZE]) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChildren = child + this->trie->getNode(oldActiveNode.node).numChildren;

    unsigned tempSize = oldActiveNode.level + 1;

    for (; child < endChildren; child++) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        State* newState = this->getNewState(queryLength,
                                            oldActiveNode.state,
                                            tempSize,
                                            this->trie->getNode(child).getValue(),
                                            bitmaps);

        if (newState->isFinal) continue;

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
                this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        int currentEditDistance = newState->getEditDistance((int) queryLength - (int) tempSize);
        if (currentEditDistance <= this->editDistanceThreshold) {
            activeNodes.emplace_back(child, newState, tempSize, currentEditDistance);
        } else {
            ActiveNode tmp(child, newState, tempSize, currentEditDistance);
            this->findActiveNodes(queryLength, tmp, activeNodes, bitmaps);
        }
    }
}

void Beva::findActiveNodesWithPruningV2(unsigned queryLength,
                                        ActiveNode &oldActiveNode,
                                        vector<ActiveNode> &activeNodes,
                                        unsigned (&bitmaps)[CHAR_SIZE],
                                        TopKHeap& topKHeap) {
    unsigned child = this->trie->getNode(oldActiveNode.node).children;
    unsigned endChildren = child + this->trie->getNode(oldActiveNode.node).numChildren;
    unsigned tempSize = oldActiveNode.level + 1;

    double nodeScore = utils::dynamicScore(this->trie->getNode(oldActiveNode.node).getMaxStaticScore(),
                                            this->preCalculatedExponentiation[this->editDistanceThreshold]);
    unsigned recordIdFromNodeScore = this->trie->getNode(oldActiveNode.node).getRecordIdFromMaxScore();

    if (topKHeap.isFull() && (nodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromNodeScore))) {
        return;
    }

    for (; child < endChildren; child++) {
        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfIterationInChildren(queryLength);
        #endif

        State* newState = this->getNewState(queryLength,
                                            oldActiveNode.state,
                                            tempSize,
                                            this->trie->getNode(child).getValue(),
                                            bitmaps);

        if (newState->isFinal) {
            continue;
        }

        #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->incrementNumberOfActiveNodes(queryLength);
        #endif

        int currentEditDistance = newState->getEditDistance((int) queryLength - (int) tempSize);

        if (currentEditDistance <= this->editDistanceThreshold) {
            activeNodes.emplace_back(child, newState, tempSize, currentEditDistance);
        } else {
            ActiveNode tmp(child, newState, tempSize, currentEditDistance);
            this->findActiveNodesWithPruningV2(queryLength, tmp, activeNodes, bitmaps, topKHeap);
        }
    }
}
