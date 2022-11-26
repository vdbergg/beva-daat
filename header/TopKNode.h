//
// Created by victor on 08/04/2022.
//

#ifndef BEVA_TOPKNODE_H
#define BEVA_TOPKNODE_H

#include <vector>

class TopKNode {
public:
//    std::vector<char *> suggestion;
    unsigned recordId;
    double maxScore;

//    TopKNode(std::vector<char *> suggestion, unsigned recordId, double MaxScore)
    TopKNode(unsigned recordId, double MaxScore)
    {
//        this->setSuggestion(suggestion);
        this->setRecordId(recordId);
        this->setMaxScore(MaxScore);
    }
//    inline void setSuggestion(std::vector<char *> suggestion) { this->suggestion = suggestion; }
//    inline std::vector<char *> getSuggestion() { return this->suggestion; }
    inline void setRecordId(unsigned recordId) { this->recordId = recordId; }
    inline unsigned getRecordId() { return this->recordId; }
    inline void setMaxScore(double maxScore) { this->maxScore = maxScore; }
    inline double getMaxScore() { return this->maxScore; }
};

#endif //BEVA_TOPKNODE_H
