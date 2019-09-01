//
// Created by berg on 31/08/2019.
//

#include "../header/QueryResult.h"

QueryResult::QueryResult(long queryProcessingTime, vector<long>&prefixProcessingTime,
        vector<long>&prefixActiveNodeSizes) {
    this->queryProcessingTime = queryProcessingTime;
    this->prefixProcessingTime = prefixProcessingTime;
    this->prefixActiveNodeSizes = prefixActiveNodeSizes;
}
