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
    int kResults;
    int dataset;

    Beva* beva;
    vector<Beva*> bevaTopK;

    Framework();

    void readData(string&, vector<StaticString>&);
    void readData(string&, vector<double>&);
    void readData(string&, vector<string>&);
    void index();
    void process(string,
                 int,
                 int,
                 vector<ActiveNode>& oldActiveNodes,
                 vector<ActiveNode>& currentActiveNodes,
                 unsigned (&bitmaps)[CHAR_SIZE]);
    vector<char *> processFullQuery(string &query, int queryPosition = -1);
    vector<char *> processQuery(string &query, int queryId);
    void processQueryWithTopKBruteForce(string &query, int queryId);
    void processQueryWithTopKPruningV1(string &query, int queryId);
    vector<char *> output(vector<ActiveNode>& currentActiveNodes) const;
    void processQueryWithTopKPruningV2(string &query, int queryId);
    void buildTopKBruteForce(vector<ActiveNode>& currentActiveNodes, TopKHeap& topKHeap) const;
    void buildTopKWithPruningV1Range(vector<ActiveNode>& currentActiveNodes,
                                     TopKHeap& topKHeap);
    void processQueryWithTopKPruningV3(string &query, int queryId);
    void processFullQueryWithTopK(string &query, vector<char *>& results);
    void buildTopKWithPruningV3Range(vector<ActiveNode>& currentActiveNodes,
                                     const long long* preCalculatedExponentiation,
                                     TopKHeap& topKHeap,
                                     int currentEditDistance);
    void buildTopKWithPruningV1Traversal(vector<ActiveNode>& currentActiveNodes,
                                         double querySize,
                                         TopKHeap& topKHeap);
    void buildTopKWithPruningV2Range(vector<ActiveNode>& currentActiveNodes,
                                     const long long *preCalculatedExponentiation,
                                     TopKHeap& topKHeap,
                                     int currentEditDistance) const;
    void buildTopKWithPruningV2Traversal(vector<ActiveNode>& currentActiveNodes,
                                         const long long* preCalculatedExponentiation,
                                         TopKHeap& topKHeap,
                                         int currentEditDistance);
    void transverseTrieDFS(unsigned node, unsigned editDistance, TopKHeap& topKHeap);
    void writeExperiments();
    void quickSort(vector<ActiveNode>& activeNodes, int inicio, int fim);
    void quickSort(vector<ActiveNode>& activeNodes);
    void ordenarPorInsercao(vector<ActiveNode>& activeNodes, unsigned tamanhoDoVetor);

    ~Framework();
};


#endif //BEVA_FRAMEWORK_H
