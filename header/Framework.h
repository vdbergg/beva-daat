//
// Created by vdberg on 12/02/19.
//

#ifndef BEVA_FRAMEWORK_H
#define BEVA_FRAMEWORK_H

#include <vector>
#include <string>
#include "ActiveNode.h"
#include "Trie.h"
#include "Beva.h"
#include "Experiment.h"
#include "GlobalVariables.h"
#include "TopKHeap.h"

using namespace std;

class Framework {
public:
    Trie* trie;
    vector<string> queries;
    vector<string> relevantQueries;
    int editDistanceThreshold;
    int dataset;

    Beva* beva;
    vector<Beva*> bevaTopK;
    Beva* bevaTau0;
    Beva* bevaTau1;
    Beva* bevaTau2;

    Framework();

    void readData(string&, vector<StaticString>&);
    void readData(string&, vector<double>&);
    void readData(string&, vector<string>&);
    void index();
    void process(string, int, int, vector<ActiveNode>& oldActiveNodes, vector<ActiveNode>& currentActiveNodes,
            unsigned (&bitmaps)[CHAR_SIZE]);
    void processTopK(string,
                 int,
                 int,
                 vector<vector<ActiveNode>>& oldActiveNodes,
                 vector<vector<ActiveNode>>& currentActiveNodes,
                 unsigned (&bitmaps)[3][CHAR_SIZE],
                 TopKHeap& topKHeap);
    vector<char *> processFullQuery(string &query, int queryPosition = -1);
    vector<char *> processQuery(string &query, int queryId);
    vector<char *> output(vector<ActiveNode>& currentActiveNodes);
    vector<char *> processTopKQuery(string &query, int queryId);
    TopKHeap buildTopK(vector<ActiveNode>& currentActiveNodes, double querySize);
    void buildTopKMultiBeva(vector<ActiveNode>& currentActiveNodes, double querySize, TopKHeap& topKHeap, double editDistance);
    void writeExperiments();

    ~Framework();
};


#endif //BEVA_FRAMEWORK_H
