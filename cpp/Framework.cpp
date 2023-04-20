//
// Created by vdberg on 12/02/19.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <sys/stat.h>
#include <stdlib.h>
#include "../header/Trie.h"
#include "../header/Framework.h"
#include "../header/Directives.h"
#include "../header/TopKHeap.h"

using namespace std;

Experiment* experiment;

Framework::Framework() {
    this->trie = nullptr;
    this->editDistanceThreshold = stoi(config["edit_distance"]);
    this->kResults = stoi(config["k_results"]);
    this->dataset = stoi(config["dataset"]);
    experiment = new Experiment(config, editDistanceThreshold);

    index();
}

Framework::~Framework() {
    cout << "deleting framework" << endl;
    delete this->beva;
    delete this->trie;
    delete experiment;
    for (auto *b : this->bevaTopK) {
        delete b;
    }
}

unsigned long getFileSize(string& filename) {
    FILE *fp=fopen(filename.c_str(),"r");

    struct stat buf;
    fstat(fileno(fp), &buf);
    fclose(fp);
    return buf.st_size;
}

void Framework::readData(string& filename, vector<StaticString>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    unsigned long fileSize = getFileSize(filename);
//    cout << "Tamanho do Arquivo:" << fileSize << endl;
    char *tmpPtr = (char*) malloc(sizeof(char)*fileSize);
    StaticString::setDataBaseMemory(tmpPtr, fileSize);
    while (getline(input, str)) {
        if (!str.empty()) {
            recs.push_back(StaticString(str));
        }
    }
}

void Framework::readData(string& filename, vector<double>& recs) {
    cout << "reading score dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        if (!str.empty()) {
            recs.push_back(std::stod(str));
        }
    }
}

void Framework::readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        if (!str.empty()) recs.push_back(str);
    }
}

void Framework::index(){
    cout << "indexing... \n";
    string sizeSuffix = "";
    switch (stoi(config["size_type"])) {
        case 0:
            sizeSuffix = "_20";
            break;
        case 1:
            sizeSuffix = "_40";
            break;
        case 2:
            sizeSuffix = "_60";
            break;
        case 3:
            sizeSuffix = "_80";
            break;
        case 4:
            sizeSuffix = "";
            break;
        default:
            sizeSuffix = "";
    }

    auto start = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initIndexingTime();
    #endif
    
    string datasetFile = config["dataset_basepath"];
    string queryFile = config["query_basepath"];
    string relevantQueryFile = config["query_basepath"];
    string scoreFile = config["score_basepath"];

    int queriesSize = stoi(config["queries_size"]);
    string datasetSuffix = queriesSize == 10 ? "_10" : "";
    string tau = to_string(this->editDistanceThreshold);

    switch (this->dataset) {
        case C::AOL:
            datasetFile += "aol/aol" + sizeSuffix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            scoreFile += "jusbrasil/scores.txt";
            break;
        case C::MEDLINE:
            datasetFile += "medline/medline" + sizeSuffix + ".txt";
            queryFile += "medline/q13" + datasetSuffix + ".txt";
            break;
        case C::USADDR:
            datasetFile += "usaddr/usaddr" + sizeSuffix + ".txt";
            queryFile += "usaddr/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::MEDLINE19:
            datasetFile += "medline19/medline19" + sizeSuffix + ".txt";
            queryFile += "medline19/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::DBLP:
            datasetFile += "dblp/dblp" + sizeSuffix + ".txt";
            queryFile += "dblp/q17_" + tau + datasetSuffix + ".txt";
	          break;
        case C::UMBC:
            datasetFile += "umbc/umbc" + sizeSuffix + ".txt";
            queryFile += "umbc/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::JUSBRASIL:
            datasetFile += "jusbrasil/top-k/jusbrasil-topk" + sizeSuffix + ".txt";
            queryFile += "jusbrasil/top-k/q.txt";
            relevantQueryFile += "jusbrasil/relevant_answers.txt";
            scoreFile += "jusbrasil/top-k/scores.txt";
            break;
        default:
            datasetFile += "aol/aol" + sizeSuffix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            scoreFile += "jusbrasil/scores.txt";
            break;
    }

    readData(datasetFile, records);
    //    sort(this->records.begin(), this->records.end());
    readData(queryFile, this->queries);

    if (config["use_top_k_v1"] == "1" || config["use_top_k_v2"] == "1" || config["use_top_k_v3"] == "1"
        || config["use_top_k_v4"] == "1") {
        readData(scoreFile, recordsStaticScore);
    }

    if (config["has_relevant_queries"] == "1") {
        readData(relevantQueryFile, this->relevantQueries);
    }

    this->trie = new Trie(experiment);
    this->trie->buildDaatIndex();
    if (config["use_top_k_v1"] == "1" || config["use_top_k_v2"] == "1" || config["use_top_k_v3"] == "1"
        || config["use_top_k_v4"] == "1") {
        this->trie->buildMaxScores();
    }
    this->trie->shrinkToFit();

    long long *preCalculatedExponentiation = new long long[this->editDistanceThreshold + 1];
    for (int i = 0; i <= this->editDistanceThreshold; i++) {
        preCalculatedExponentiation[i] = utils::fast_exponentiation(5, (this->editDistanceThreshold - i));
    }
    this->beva = new Beva(this->trie, experiment, this->editDistanceThreshold, preCalculatedExponentiation);

    if (config["use_top_k_v3"] == "1" || config["use_top_k_v4"] == "1") {
        for (int i = 1; i <= this->editDistanceThreshold; i++) {
            preCalculatedExponentiation = new long long[i + 1];
            for (int j = 0; j <= i; j++) {
                preCalculatedExponentiation[j] = utils::fast_exponentiation(5, (i - j));
            }
            this->bevaTopK.push_back(new Beva(this->trie, experiment, i, preCalculatedExponentiation));
        }
    }

    auto done = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        this->experiment->getMemoryUsedInIndexing();
    #else
        experiment->endIndexingTime();
        experiment->compileProportionOfBranchingSizeInBEVA2Level();
        experiment->compileNumberOfNodes();
    #endif
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}

vector<char *> Framework::processFullQuery(string &query, int queryPosition) {
    #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initQueryProcessingTime();
    #endif

    vector<ActiveNode> currentActiveNodes;
    vector<ActiveNode> oldActiveNodes;

    unsigned bitmaps[CHAR_SIZE];
    for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
        swap(oldActiveNodes, currentActiveNodes);
        currentActiveNodes.clear();
        this->beva->process(query[currentPrefixQuery - 1], currentPrefixQuery, oldActiveNodes,
                currentActiveNodes, bitmaps);
        currentActiveNodes.shrink_to_fit();
        oldActiveNodes.clear();
    }

    #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->endSimpleQueryProcessingTime(currentActiveNodes.size());
        experiment->initQueryFetchingTime();
    #endif

    vector<char *> results = this->output(currentActiveNodes);

    #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->endSimpleQueryFetchingTime(results.size());

        bool relevantReturned = false;
        if (queryPosition != -1 && config["has_relevant_queries"] == "1") {
            vector<string> v_output;
            v_output.resize(results.size());
            copy(results.begin(), results.end(), v_output.begin());

            relevantReturned = find(v_output.begin(), v_output.end(),
                    this->relevantQueries[queryPosition]) != v_output.end();
        }
        experiment->compileSimpleQueryProcessingTimes(query, relevantReturned);
    #endif

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        this->experiment->getMemoryUsedInProcessing();
    #endif

    return results;
}

vector<char *> Framework::processQuery(string &query, int queryId) {
    vector<ActiveNode> currentActiveNodes;
    vector<ActiveNode> oldActiveNodes;

    unsigned bitmaps[CHAR_SIZE];
    for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
        swap(oldActiveNodes, currentActiveNodes);
        currentActiveNodes.clear();
        this->process(query, currentPrefixQuery, queryId, oldActiveNodes, currentActiveNodes, bitmaps);
        oldActiveNodes.clear();
    }

    vector<char *> results = this->output(currentActiveNodes);

    return results;
}

void Framework::process(string query,
                        int prefixQueryLength,
                        int currentCountQuery,
                        vector<ActiveNode>& oldActiveNodes,
                        vector<ActiveNode>& currentActiveNodes,
                        unsigned (&bitmaps)[CHAR_SIZE]) {
    if (query.empty()) return;

#ifdef BEVA_IS_COLLECT_TIME_H
    experiment->initQueryProcessingTime();
#endif

    this->beva->process(query[prefixQueryLength - 1],
                        prefixQueryLength,
                        oldActiveNodes,
                        currentActiveNodes,
                        bitmaps);

#ifdef BEVA_IS_COLLECT_TIME_H
    experiment->endQueryProcessingTime(currentActiveNodes.size(), prefixQueryLength);

    vector<int> prefixesSize = { 5, 9, 13, 17 };
    if (std::find(prefixesSize.begin(), prefixesSize.end(), prefixQueryLength) != prefixesSize.end()) {
        experiment->initQueryFetchingTime();
        vector<char *> results = output(currentActiveNodes);
        experiment->endQueryFetchingTime(prefixQueryLength, results.size());
    }
#endif

    currentActiveNodes.shrink_to_fit();
    if (query.length() == prefixQueryLength) {
#ifdef BEVA_IS_COLLECT_MEMORY_H
        this->experiment->getMemoryUsedInProcessing();
#else
        experiment->compileQueryProcessingTimes(currentCountQuery);
        string currentQuery = query.substr(0, prefixQueryLength);
        experiment->saveQueryProcessingTime(currentQuery, currentCountQuery);
#endif
    }
}

void Framework::processQueryWithTopKBruteForce(string &query, int queryId) {
    vector<ActiveNode> currentActiveNodes;
    vector<ActiveNode> oldActiveNodes;

    unsigned bitmaps[CHAR_SIZE];
    for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
        swap(oldActiveNodes, currentActiveNodes);
        currentActiveNodes.clear();

        #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initQueryProcessingTime();
        #endif

        this->beva->process(query[currentPrefixQuery - 1],
                            currentPrefixQuery,
                            oldActiveNodes,
                            currentActiveNodes,
                            bitmaps);

        #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->endQueryProcessingTime(currentActiveNodes.size(), currentPrefixQuery);

        vector<int> prefixesSize = { 5, 9, 13, 17 };
        if (std::find(prefixesSize.begin(), prefixesSize.end(), currentPrefixQuery) != prefixesSize.end()) {
            experiment->initQueryFetchingTime();
            TopKHeap topKHeap(this->kResults);
            buildTopKBruteForce(currentActiveNodes, topKHeap);
            vector<char *> results = topKHeap.outputSuggestions();
            experiment->endQueryFetchingTime(currentPrefixQuery, results.size());
        }
        #endif

        currentActiveNodes.shrink_to_fit();
        if (query.length() == currentPrefixQuery) {
            #ifdef BEVA_IS_COLLECT_MEMORY_H
            this->experiment->getMemoryUsedInProcessing();
            #else
            experiment->compileQueryProcessingTimes(queryId);
            string currentQuery = query.substr(0, currentPrefixQuery);
            experiment->saveQueryProcessingTime(currentQuery, queryId);
            #endif
        }

        oldActiveNodes.clear();
    }
}

void Framework::processQueryWithTopKPruningV1(string &query, int queryId) {
    vector<ActiveNode> currentActiveNodes;
    vector<ActiveNode> oldActiveNodes;

    unsigned bitmaps[CHAR_SIZE];
    for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
        swap(oldActiveNodes, currentActiveNodes);
        currentActiveNodes.clear();

        #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initQueryProcessingTime();
        #endif

        this->beva->process(query[currentPrefixQuery - 1],
                            currentPrefixQuery,
                            oldActiveNodes,
                            currentActiveNodes,
                            bitmaps);

        #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->endQueryProcessingTime(currentActiveNodes.size(), currentPrefixQuery);

        vector<int> prefixesSize = { 5, 9, 13, 17 };
        if (std::find(prefixesSize.begin(), prefixesSize.end(), currentPrefixQuery) != prefixesSize.end()) {
            experiment->initQueryFetchingTime();
            TopKHeap topKHeap(this->kResults);
            buildTopKWithPruningV1Range(currentActiveNodes, topKHeap);
            vector<char *> results = topKHeap.outputSuggestions();
            experiment->endQueryFetchingTime(currentPrefixQuery, results.size());
        }
        #endif

        currentActiveNodes.shrink_to_fit();
        if (query.length() == currentPrefixQuery) {
            #ifdef BEVA_IS_COLLECT_MEMORY_H
            this->experiment->getMemoryUsedInProcessing();
            #else
            experiment->compileQueryProcessingTimes(queryId);
            string currentQuery = query.substr(0, currentPrefixQuery);
            experiment->saveQueryProcessingTime(currentQuery, queryId);
            #endif
        }

        oldActiveNodes.clear();
    }
}

void Framework::processQueryWithTopKPruningV2(string &query, int queryId) {
    vector<vector<ActiveNode>> currentActiveNodes;
    vector<vector<ActiveNode>> oldActiveNodes;
    unsigned bitmaps[4][CHAR_SIZE];

    for (int i = 0; i <= this->editDistanceThreshold; i++) {
        currentActiveNodes.emplace_back(vector<ActiveNode>());
        oldActiveNodes.emplace_back(vector<ActiveNode>());

        if (i > 0) {
            for (auto &bitmap : bitmaps[i]) {
                bitmap = this->bevaTopK[i - 1]->bitmapZero;
            }
        }
    }
    oldActiveNodes[0].emplace_back(this->trie->root, nullptr, 0, 0);

    TopKHeap topKHeap(this->kResults);
    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
        swap(oldActiveNodes, currentActiveNodes);
        for (int i = 0; i <= this->editDistanceThreshold; i++) {
            currentActiveNodes[i].clear();
        }

        #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initQueryProcessingTime();
        #endif

        this->beva->processNoErrors(query[currentPrefixQuery - 1],
                                    currentPrefixQuery,
                                    oldActiveNodes[0],
                                    currentActiveNodes[0]);

        for (int i = 1; i <= this->editDistanceThreshold; i++) {
            this->bevaTopK[i - 1]->processWithPruningV2(query[currentPrefixQuery - 1],
                                                        currentPrefixQuery,
                                                        oldActiveNodes[i],
                                                        currentActiveNodes[i],
                                                        bitmaps[i],
                                                        topKHeap);
        }

        #ifdef BEVA_IS_COLLECT_TIME_H
        int numberOfActiveNodes = 0;
        for (vector<ActiveNode>& activeNodes : currentActiveNodes) {
            numberOfActiveNodes += activeNodes.size();
        }
        experiment->endQueryProcessingTime(numberOfActiveNodes, currentPrefixQuery);

        vector<int> prefixesSize = { 5, 9, 13, 17 };
        bool isToFetch = std::find(prefixesSize.begin(), prefixesSize.end(), currentPrefixQuery) != prefixesSize.end();
        if (isToFetch) {
            experiment->initQueryFetchingTime();

            buildTopKWithPruningV2Range(currentActiveNodes[0],
                                        this->beva->preCalculatedExponentiation,
                                        topKHeap,
                                        0);

            for (int i = 1; i <= this->editDistanceThreshold; i++) {
                buildTopKWithPruningV2Range(currentActiveNodes[i],
                                            this->bevaTopK[i - 1]->preCalculatedExponentiation,
                                            topKHeap,
                                            i);
            }
            vector<char *> results = topKHeap.outputSuggestions();
            experiment->endQueryFetchingTime(currentPrefixQuery, results.size());
        }
        #endif

        currentActiveNodes.shrink_to_fit();
        if (query.length() == currentPrefixQuery) {
            #ifdef BEVA_IS_COLLECT_MEMORY_H
            this->experiment->getMemoryUsedInProcessing();
            #else
            experiment->compileQueryProcessingTimes(queryId);
            string currentQuery = query.substr(0, currentPrefixQuery);
            experiment->saveQueryProcessingTime(currentQuery, queryId);
            #endif
        }

        for (int i = 0; i <= this->editDistanceThreshold; i++) {
            oldActiveNodes[i].clear();
        }
    }
}

void Framework::processQueryWithTopKPruningV3(string &query, int queryId) {
    TopKHeap topKHeap(this->kResults);

    for (int ed = 0; ed <= this->editDistanceThreshold; ed++) {
        vector<ActiveNode> currentActiveNodes;
        vector<ActiveNode> oldActiveNodes;

        unsigned bitmaps[CHAR_SIZE];
        for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

        for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
            swap(oldActiveNodes, currentActiveNodes);
            currentActiveNodes.clear();

            #ifdef BEVA_IS_COLLECT_TIME_H
            experiment->initQueryProcessingTime();
            #endif

            if (ed == 0) {
                this->beva->processNoErrors(query[currentPrefixQuery - 1],
                                            currentPrefixQuery,
                                            oldActiveNodes,
                                            currentActiveNodes);
            } else {
                this->bevaTopK[ed - 1]->processWithPruningV2(query[currentPrefixQuery - 1],
                                                           currentPrefixQuery,
                                                           oldActiveNodes,
                                                           currentActiveNodes,
                                                           bitmaps,
                                                           topKHeap);
            }

            #ifdef BEVA_IS_COLLECT_TIME_H
            experiment->endQueryProcessingTime(currentActiveNodes.size(), currentPrefixQuery);

            vector<int> prefixesSize = {5, 9, 13, 17};
            if (std::find(prefixesSize.begin(), prefixesSize.end(), currentPrefixQuery) != prefixesSize.end()) {
                experiment->initQueryFetchingTime();
                buildTopKWithPruningV3Range(currentActiveNodes,
                                            ed == 0 ? this->beva->preCalculatedExponentiation
                                                    : this->bevaTopK[ed - 1]->preCalculatedExponentiation,
                                            topKHeap,
                                            ed);
                vector<char *> results = topKHeap.outputSuggestions();
                experiment->endQueryFetchingTime(currentPrefixQuery, results.size());
            }
            #endif

            currentActiveNodes.shrink_to_fit();
            oldActiveNodes.clear();
        }
    }

    #ifdef BEVA_IS_COLLECT_MEMORY_H
    this->experiment->getMemoryUsedInProcessing();
    #else
    experiment->compileQueryProcessingTimes(queryId, true);
    experiment->saveQueryProcessingTime(query, queryId, true);
    #endif
}

void Framework::processFullQueryWithTopK(string &query, vector<char *>& results) {
    TopKHeap topKHeap(this->kResults);
    vector<ActiveNode> currentActiveNodes;
    vector<ActiveNode> oldActiveNodes;

    for (int ed = 0; ed <= this->editDistanceThreshold; ed++) {
        currentActiveNodes.clear();

        unsigned bitmaps[CHAR_SIZE];
        for (auto & bitmap : bitmaps) bitmap = this->beva->bitmapZero;

        for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
            swap(oldActiveNodes, currentActiveNodes);
            currentActiveNodes.clear();

            if (ed == 0) {
                this->beva->processNoErrors(query[currentPrefixQuery - 1],
                                            currentPrefixQuery,
                                            oldActiveNodes,
                                            currentActiveNodes);
            } else {
                this->bevaTopK[ed - 1]->processWithPruningV2(query[currentPrefixQuery - 1],
                                                             currentPrefixQuery,
                                                             oldActiveNodes,
                                                             currentActiveNodes,
                                                             bitmaps,
                                                             topKHeap);
            }

            currentActiveNodes.shrink_to_fit();
            oldActiveNodes.clear();
        }

        buildTopKWithPruningV3Range(currentActiveNodes,
                                    ed == 0 ? this->beva->preCalculatedExponentiation
                                            : this->bevaTopK[ed - 1]->preCalculatedExponentiation,
                                    topKHeap,
                                    ed);
    }

    topKHeap.outputSuggestions(results);
}

void Framework::buildTopKWithPruningV3Range(vector<ActiveNode>& currentActiveNodes,
                                            const long long* preCalculatedExponentiation,
                                            TopKHeap& topKHeap,
                                            int currentEditDistance) {
    this->quickSort(currentActiveNodes);

    for (int i = 0; i < topKHeap.capacity && i < currentActiveNodes.size(); i++) {
        double activeNodeScore = this->trie->getNode(currentActiveNodes[i].node).getMaxStaticScore();
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(currentActiveNodes[i].node).getRecordIdFromMaxScore();

        if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

        unsigned beginRange = this->trie->getNode(currentActiveNodes[i].node).getBeginRange();
        unsigned endRange = this->trie->getNode(currentActiveNodes[i].node).getEndRange();
        long long score = preCalculatedExponentiation[currentEditDistance];

        for (unsigned i = beginRange; i < endRange; i++) {
            double dynamicScore = utils::dynamicScore(recordsStaticScore[i], score);
            TopKNode nodeToInsert(i, dynamicScore);
            topKHeap.insertNode(nodeToInsert);
        }
    }
}

void Framework::writeExperiments() {
    #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->compileNumberOfActiveNodes();
        this->experiment->compileNumberOfIterationInChildren();
    #endif
}

void Framework::buildTopKBruteForce(vector<ActiveNode>& currentActiveNodes,
                                    TopKHeap& topKHeap) const {
    for (ActiveNode activeNode : currentActiveNodes) {
        unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();

        for (unsigned i = beginRange; i < endRange; i++) {
            double dynamicScore = utils::dynamicScore(recordsStaticScore[i],
                                                      this->beva->preCalculatedExponentiation[activeNode.editDistance]);
            TopKNode nodeToInsert(i, dynamicScore);
            topKHeap.insertNode(nodeToInsert);
        }
    }
}

void Framework::buildTopKWithPruningV2Range(vector<ActiveNode>& currentActiveNodes,
                                            const long long* preCalculatedExponentiation,
                                            TopKHeap& topKHeap,
                                            int currentEditDistance) const {
    for (ActiveNode activeNode : currentActiveNodes) {
        if (currentEditDistance == activeNode.editDistance) {
            double activeNodeScore = this->trie->getNode(activeNode.node).getMaxStaticScore();
            unsigned recordIdFromActiveNodeScore = this->trie->getNode(activeNode.node).getRecordIdFromMaxScore();

            if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

            unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
            unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();
            long long score = preCalculatedExponentiation[activeNode.editDistance];

            for (unsigned i = beginRange; i < endRange; i++) {
                double dynamicScore = utils::dynamicScore(recordsStaticScore[i], score);

                TopKNode nodeToInsert(i, dynamicScore);
                topKHeap.insertNode(nodeToInsert);
            }
        }
    }
}

const int limiarInsercao = 30;

/* Função para ordenar um vetor de float por inserção (insertion sort) */

void Framework::ordenarPorInsercao(vector<ActiveNode>& activeNodes, unsigned tamanhoDoVetor) {
    int i, j;

    for (i = 1; i < tamanhoDoVetor; i++) {
        ActiveNode pivot = activeNodes[i];
        j = i - 1;

        while ((j >= 0) && ( this->trie->getNode(activeNodes[j].node).getMaxStaticScore() < this->trie->getNode(pivot.node).getMaxStaticScore()) ) {
            activeNodes[j + 1] = activeNodes[j];
            j--;
        }
        activeNodes[j + 1] = pivot;
    }
}

void Framework::quickSort(vector<ActiveNode>& activeNodes, int inicio, int fim) {
    int i, j;

    ActiveNode pivot = activeNodes[(inicio + fim) / 2];
    i = inicio;
    j = fim;

    while (i <= j) {
        while (this->trie->getNode(activeNodes[i].node).getMaxStaticScore() > this->trie->getNode(pivot.node).getMaxStaticScore()) { i++; }  // avanca enquanto está do lado certo
        while (this->trie->getNode(activeNodes[j].node).getMaxStaticScore() < this->trie->getNode(pivot.node).getMaxStaticScore()) { j--; } // recua enquanto está do lado certo

        if (i <= j) {
            ActiveNode tmp = activeNodes[i];
            activeNodes[i] = activeNodes[j];
            activeNodes[j] = tmp;
            i++;
            j--;
        }
    }
    if ( (j - inicio) > limiarInsercao) { quickSort(activeNodes, inicio, j); }
    if ( (fim - i) > limiarInsercao) { quickSort(activeNodes, i, fim); }
}


void Framework::quickSort(vector<ActiveNode>& activeNodes) {
    int tamanhoDoVetor = activeNodes.size();

    if (tamanhoDoVetor > limiarInsercao) {
        this->quickSort(activeNodes, 0,  tamanhoDoVetor - 1);
    }

    ordenarPorInsercao(activeNodes, tamanhoDoVetor);
}

void Framework::buildTopKWithPruningV1Range(vector<ActiveNode>& currentActiveNodes,
                                            TopKHeap& topKHeap) {
    this->quickSort(currentActiveNodes);

    for (int i = 0; i < topKHeap.capacity && i < currentActiveNodes.size(); i++) {
        double activeNodeScore = this->trie->getNode(currentActiveNodes[i].node).getMaxStaticScore();
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(currentActiveNodes[i].node).getRecordIdFromMaxScore();

        if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

        unsigned beginRange = this->trie->getNode(currentActiveNodes[i].node).getBeginRange();
        unsigned endRange = this->trie->getNode(currentActiveNodes[i].node).getEndRange();
        long long score = this->beva->preCalculatedExponentiation[currentActiveNodes[i].editDistance];

        for (unsigned i = beginRange; i < endRange; i++) {
            double dynamicScore = utils::dynamicScore(recordsStaticScore[i], score);
            TopKNode nodeToInsert(i, dynamicScore);
            topKHeap.insertNode(nodeToInsert);
        }
    }
}

void Framework::transverseTrieDFS(unsigned node, unsigned editDistance, TopKHeap& topKHeap) {
    unsigned child = this->trie->getNode(node).children;
    unsigned endChildren = child + this->trie->getNode(node).numChildren;

    for (; child < endChildren; child++) {
        double activeNodeScore = this->trie->getNode(node).getMaxStaticScore();
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(node).getRecordIdFromMaxScore();

        if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

        if (this->trie->getNode(child).getIsEndOfWord()) {
            unsigned recordId = this->trie->getNode(child).getBeginRange();
            double dynamicScore = utils::dynamicScore(recordsStaticScore[recordId],
                                                      this->beva->preCalculatedExponentiation[editDistance]);
            TopKNode nodeToInsert(recordId, dynamicScore);
            topKHeap.insertNode(nodeToInsert);
        } else {
            this->transverseTrieDFS(child, editDistance, topKHeap);
        }
    }
}

void Framework::buildTopKWithPruningV1Traversal(vector<ActiveNode>& currentActiveNodes,
                                                double querySize,
                                                TopKHeap& topKHeap) {
    for (ActiveNode activeNode : currentActiveNodes) {
        double activeNodeScore = this->trie->getNode(activeNode.node).getMaxStaticScore();
        unsigned recordIdFromActiveNodeScore = this->trie->getNode(activeNode.node).getRecordIdFromMaxScore();

        if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

        if (this->trie->getNode(activeNode.node).getIsEndOfWord()) {
            unsigned recordId = this->trie->getNode(activeNode.node).getBeginRange();
            double dynamicScore = utils::dynamicScore(recordsStaticScore[recordId],
                                                      this->beva->preCalculatedExponentiation[activeNode.editDistance]);
            TopKNode nodeToInsert(recordId, dynamicScore);
            topKHeap.insertNode(nodeToInsert);
        } else {
            this->transverseTrieDFS(activeNode.node, activeNode.editDistance, topKHeap);
        }
    }
}

void Framework::buildTopKWithPruningV2Traversal(vector<ActiveNode>& currentActiveNodes,
                                                const long long* preCalculatedExponentiation,
                                                TopKHeap& topKHeap,
                                                int currentEditDistance) {
    for (ActiveNode activeNode : currentActiveNodes) {
        if (activeNode.editDistance == currentEditDistance) {
            double activeNodeScore = this->trie->getNode(activeNode.node).getMaxStaticScore();
            unsigned recordIdFromActiveNodeScore = this->trie->getNode(activeNode.node).getRecordIdFromMaxScore();

            if (topKHeap.isFull() && (activeNodeScore < topKHeap.topMaxScore() || topKHeap.contains(recordIdFromActiveNodeScore))) continue;

            if (this->trie->getNode(activeNode.node).getIsEndOfWord()) {
                unsigned recordId = this->trie->getNode(activeNode.node).getBeginRange();
                double dynamicScore = utils::dynamicScore(recordsStaticScore[recordId],
                                                          preCalculatedExponentiation[activeNode.editDistance]);
                TopKNode nodeToInsert(recordId, dynamicScore);
                topKHeap.insertNode(nodeToInsert);
            } else {
                this->transverseTrieDFS(activeNode.node, activeNode.editDistance, topKHeap);
            }
        }
    }
}

vector<char *> Framework::output(vector<ActiveNode>& currentActiveNodes) const {
    vector<char *> outputs;
    string tmp;
//    int limit = 100;

    for (ActiveNode activeNode : currentActiveNodes) {
        unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();

        for (unsigned i = beginRange; i < endRange; i++) {
            outputs.push_back(records[i].c_str());
//            if (outputs.size() >= limit) return outputs;
        }
    }

    //    for (const string& record : outputs) {
    //     cout << record << "\n";
    // }

    return outputs;
}
