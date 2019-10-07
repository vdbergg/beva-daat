//
// Created by berg on 31/08/2019.
//

#ifndef BEVA_QUERYRESULT_H
#define BEVA_QUERYRESULT_H

#include <vector>
#include <string>

using namespace std;

class QueryResult {
public:
    QueryResult(long, vector<long>&, vector<long>&, int);

    long queryProcessingTime;
    int queryId;
    vector<long> prefixProcessingTime;
    vector<long> prefixActiveNodeSizes;
};


#endif //BEVA_QUERYRESULT_H
