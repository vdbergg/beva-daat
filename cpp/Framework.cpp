//
// Created by vdberg on 12/02/19.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <sys/stat.h>
#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/Framework.h"
#include "../header/utils.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"
#include "../header/TopKHeap.h"
#include "../header/TopKNode.h"

using namespace std;

Experiment* experiment;

Framework::Framework() {
    this->trie = nullptr;
    this->editDistanceThreshold = stoi(config["edit_distance"]);
    this->dataset = stoi(config["dataset"]);
    experiment = new Experiment(config, editDistanceThreshold);

    index();
}

Framework::~Framework() {
    cout << "deleting framework" << endl;
    delete this->beva;
    delete this->trie;
    delete experiment;
}

unsigned long getFileSize(string filename) {
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
    StaticString::setDataBaseMemory(tmpPtr,fileSize);
    while (getline(input, str)) {
//        for (char &c : str) {
//            if ((int) c == -61) continue;
//            else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
//                c = utils::convertSpecialCharToSimpleChar(c);
//            }
//            c = tolower(c);
//        }
        if (!str.empty()) recs.push_back(StaticString(str));
    }
}

void Framework::readData(string& filename, vector<double>& recs) {
    cout << "reading score dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    int i = 0;
    while (getline(input, str)) {
        if (!str.empty()) recs.push_back(std::stod(str));
        i += 1;
    }
}

void Framework::readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
//        for (char &c : str) {
//            if ((int) c == -61) continue;
//            else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
//                c = utils::convertSpecialCharToSimpleChar(c);
//            }
//            c = tolower(c);
//        }
        if (!str.empty()) recs.push_back(str);
    }
}

void Framework::index(){
    cout << "indexing... \n";
    string sizeSufix = "";
    switch (stoi(this->config["size_type"])) {
        case 0:
            sizeSufix = "_20";
            break;
        case 1:
            sizeSufix = "_40";
            break;
        case 2:
            sizeSufix = "_60";
            break;
        case 3:
            sizeSufix = "_80";
            break;
        case 4:
            sizeSufix = "";
            break;
        default:
            sizeSufix = "";
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
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::MEDLINE:
            datasetFile += "medline/medline" + sizeSufix + ".txt";
            queryFile += "medline/q13" + datasetSuffix + ".txt";
            break;
        case C::USADDR:
            datasetFile += "usaddr/usaddr" + sizeSufix + ".txt";
            queryFile += "usaddr/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::MEDLINE19:
            datasetFile += "medline19/medline19" + sizeSufix + ".txt";
            queryFile += "medline19/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::DBLP:
            datasetFile += "dblp/dblp" + sizeSufix + ".txt";
            queryFile += "dblp/q17_" + tau + datasetSuffix + ".txt";
	    break;
        case C::UMBC:
            datasetFile += "umbc/umbc" + sizeSufix + ".txt";
            queryFile += "umbc/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::JUSBRASIL:
            datasetFile += "jusbrasil/jusbrasil" + sizeSufix + ".txt";
            queryFile += "jusbrasil/q.txt";
            relevantQueryFile += "jusbrasil/relevant_answers.txt";
            break;
        case C::TCC:
//            datasetFile += "tcc-dataset/tcc-suggestions" + sizeSufix + ".txt";
            datasetFile += "tcc-dataset/tcc-suggestions-no-special.txt";
//            datasetFile += "tcc-dataset/tcc-suggestions_one-query.txt";
//            queryFile += "tcc-dataset/tcc-queries.txt";
            queryFile += "tcc-dataset/tcc-queries_17_1000_no_special.txt";
//            queryFile += "tcc-dataset/tcc-queries_one-query.txt";
            scoreFile += "tcc-dataset/tcc-static-score-no-special.txt";
            break;
        case C::TCCSMALL:
            datasetFile += "tcc-dataset/tcc-suggestions_small.txt";
            queryFile += "tcc-dataset/tcc-queries_small.txt";
            scoreFile += "tcc-dataset/tcc-static-score_small.txt";
            break;
        default:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            break;
    }

    readData(datasetFile, records);
    //    sort(this->records.begin(), this->records.end());
    readData(queryFile, this->queries);

    if(config["use_top_k"] == "1") {
        readData(scoreFile, recordsStaticScore);
    }

    if (config["has_relevant_queries"] == "1") {
        readData(relevantQueryFile, this->relevantQueries);
    }

    this->trie = new Trie(experiment);
    this->trie->buildDaatIndex();
    if(config["use_top_k"] == "1") this->trie->buildMaxScores();
    this->trie->shrinkToFit();


    this->beva = new Beva(this->trie, experiment, this->editDistanceThreshold);

    for (int i = 0; i < 3; i++) {
        this->bevaTopK.push_back(new Beva(this->trie, experiment, i));
    }

//     this->bevaTau0 = new Beva(this->trie, experiment, 0);
//     this->bevaTau1 = new Beva(this->trie, experiment, 1);
//     this->bevaTau2 = new Beva(this->trie, experiment, 2);

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

    // processando os nós ativos da
    vector<char *> results = this->output(currentActiveNodes);

    return results;
}

void Framework::process(string query,
                        int prefixQueryLength,
                        int currentCountQuery,
                        vector<ActiveNode>& oldActiveNodes,
                        vector<ActiveNode>& currentActiveNodes,
                        unsigned (&bitmaps)[CHAR_SIZE]
                        ) {
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

        vector<int> prefixQuerySizeToFetching = { 5, 9, 13, 17 };
        if (std::find(prefixQuerySizeToFetching.begin(), prefixQuerySizeToFetching.end(), prefixQueryLength) !=
            prefixQuerySizeToFetching.end())
        {
            experiment->initQueryFetchingTime();
//            vector<char *> results = output(currentActiveNodes);
            TopKHeap topKHeap = buildTopK(currentActiveNodes, prefixQueryLength);
            vector<char *> results = topKHeap.outputSuggestions();
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

vector<char *> Framework::processTopKQuery(string &query, int queryId) {

    vector<vector<ActiveNode>> currentActiveNodes;
    vector<vector<ActiveNode>> oldActiveNodes;
    unsigned bitmaps[3][CHAR_SIZE];

    for(int i = 0; i < 3; i++) {
        for (auto & bitmap : bitmaps[i]) {
            bitmap = this->bevaTopK[i]->bitmapZero;
        }
        currentActiveNodes.emplace_back(vector<ActiveNode>());
        oldActiveNodes.emplace_back(vector<ActiveNode>());
    }

    for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
//        cout << "Prefixo: " << currentPrefixQuery << endl;
        TopKHeap heap;
        swap(oldActiveNodes, currentActiveNodes);

        for(int i = 0; i < 3; i++) {
            currentActiveNodes[i].clear();
        }


        this->processTopK(query,
                      currentPrefixQuery,
                      queryId,
                      oldActiveNodes,
                      currentActiveNodes,
                      bitmaps,
                      heap);
        for(int i = 0; i < 3; i++) {
            oldActiveNodes[i].clear();
        }
    }

    // processando os nós ativos da
//    vector<char *> results = this->output(currentActiveNodes);
//    só pro editor nao encher o saco
    vector<char *> results;

    return results;
}

void Framework::processTopK(string query,
                        int prefixQueryLength,
                        int currentCountQuery,
                        vector<vector<ActiveNode>>& oldActiveNodes,
                        vector<vector<ActiveNode>>& currentActiveNodes,
                        unsigned (&bitmaps)[3][CHAR_SIZE],
                        TopKHeap& topKHeap){

    if (query.empty()) return;
    #ifdef BEVA_IS_COLLECT_TIME_H
        experiment->initQueryProcessingTime();
    #endif

    for(int i = 0; i < 3; i++) {

//        cout << "Processing Beva " << i << endl;
        this->bevaTopK[i]->processTopK(query[prefixQueryLength - 1],
                                   prefixQueryLength,
                                   oldActiveNodes,
                                   currentActiveNodes,
                                   bitmaps,
                                   topKHeap);
        buildTopKMultiBeva(currentActiveNodes[i], prefixQueryLength, topKHeap, i);
        currentActiveNodes[i].shrink_to_fit();
    }
    #ifdef BEVA_IS_COLLECT_TIME_H
    // checar analise de nodos ativos dos bevas
    experiment->endQueryProcessingTime(
            currentActiveNodes[0].size() + currentActiveNodes[1].size() + currentActiveNodes[2].size(),
            prefixQueryLength);
    vector<int> prefixQuerySizeToFetching = { 5, 9, 13, 17 };

    if (std::find(prefixQuerySizeToFetching.begin(), prefixQuerySizeToFetching.end(), prefixQueryLength) !=
        prefixQuerySizeToFetching.end())
    {
        experiment->initQueryFetchingTime();
        vector<char *> results = topKHeap.outputSuggestions();
        experiment->endQueryFetchingTime(prefixQueryLength, results.size());
    }

    #endif

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

void Framework::writeExperiments() {
    #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->compileNumberOfActiveNodes();
        this->experiment->compileNumberOfIterationInChildren();
    #endif
}

TopKHeap Framework::buildTopK(vector<ActiveNode>& currentActiveNodes, double querySize) {
    TopKHeap heap;
    string tmp;
//    int limit = 100;

    for (ActiveNode activeNode : currentActiveNodes) {
        unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();

        for (unsigned i = beginRange; i < endRange; i++) {
            double dynamicScore = utils::dynamicScore(recordsStaticScore[i],
                                                      this->editDistanceThreshold,
                                                      querySize,
                                                      this->editDistanceThreshold);
            TopKNode nodeToInsert(i, dynamicScore);
            heap.insertNode(nodeToInsert);
        }
    }


    return heap;
}

void Framework::buildTopKMultiBeva(vector<ActiveNode>& currentActiveNodes, double querySize, TopKHeap& topKHeap, double editDistance) {

    for (ActiveNode activeNode : currentActiveNodes) {
        double activeNodeScore = this->trie->getNode(activeNode.node).maxStaticScore;

        if (topKHeap.heap.size() >= 10 &&
            activeNodeScore < topKHeap.heap.front().maxScore) continue;

        unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();
        for (unsigned i = beginRange; i < endRange; i++) {
            double dynamicScore = utils::dynamicScore(recordsStaticScore[i],
                                                      editDistance,
                                                      querySize,
                                                      2);
            TopKNode nodeToInsert(i, dynamicScore);
//            cout << "executando o insert Node" << endl;
            topKHeap.insertNode(nodeToInsert);
//            cout << "nó inserido" << endl;
        }
    }

//    cout << "em teoria rodou o for" << endl;
}

vector<char *> Framework::output(vector<ActiveNode>& currentActiveNodes) {
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
