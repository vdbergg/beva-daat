//
// Created by vdberg on 12/02/19.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/Framework.h"
#include "../header/utils.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"

using namespace std;

Framework::Framework(unordered_map<string, string> config) {
    this->trie = nullptr;
    this->editDistanceThreshold = stoi(config["edit_distance"]);
    this->dataset = stoi(config["dataset"]);
    this->experiment = new Experiment(config, editDistanceThreshold);
    this->config = config;

    index();
}

Framework::~Framework() {
    cout << "deleting framework" << endl;
    delete this->beva;
    delete this->trie;
    delete this->experiment;
}

void Framework::readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        for (char &c : str) {
            if ((int) c == -61) continue;
            else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
                c = utils::convertSpecialCharToSimpleChar(c);
            }
            c = tolower(c);
        }
        if (!str.empty()) recs.push_back(str);
    }
}

void Framework::index(){
    cout << "indexing... \n";
    string sizeSufix = "";
    switch (stoi(this->config["size_type"])) {
        case 0:
            sizeSufix = "_25";
            break;
        case 1:
            sizeSufix = "_50";
            break;
        case 2:
            sizeSufix = "_75";
            break;
        case 3:
            sizeSufix = "";
    }

    auto start = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->initIndexingTime();
    #endif
    
    string datasetFile = this->config["dataset_basepath"];
    string queryFile = this->config["query_basepath"];

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
        default:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            break;
    }

    readData(datasetFile, this->records);
//    sort(this->records.begin(), this->records.end());
    readData(queryFile, this->queries);

    this->trie = new Trie(this->records, this->experiment);
    #ifdef BEVA_IS_BUILD_INDEX_BFS_H
        this->trie->buildBfsIndex();
    #else
        this->trie->buildDfsIndex();
    #endif

    this->trie->shrinkToFit();

    this->beva = new Beva(this->trie, this->experiment, this->editDistanceThreshold);

    auto done = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        this->experiment->getMemoryUsedInIndexing();
    #else
        this->experiment->endIndexingTime();
        this->experiment->compileProportionOfBranchingSizeInBEVA2Level();
        this->experiment->compileNumberOfNodes();
    #endif
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}


void Framework::process(string query, int queryLength, int currentCountQuery) {
    if (query.empty()) return;

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->initQueryProcessingTime();
    #endif

    this->beva->process(query);

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->endQueryProcessingTime(this->beva->currentActiveNodes.size(), query);

        if (query.size() == 5 || query.size() == 9 || query.size() == 13 || query.size() == 17) {
            this->experiment->initQueryFetchingTime();
            int resultsSize = output();
            this->experiment->endQueryFetchingTime(query, resultsSize);
        }
    #endif

    this->beva->currentActiveNodes.shrink_to_fit();
    if (query.length() == queryLength) {
        #ifdef BEVA_IS_COLLECT_MEMORY_H
            this->experiment->getMemoryUsedInProcessing();
        #else
            this->experiment->compileQueryProcessingTimes(currentCountQuery);
            this->experiment->saveQueryProcessingTime(query, currentCountQuery);
        #endif
        this->beva->reset(); // Reset the information from previous query
    }
}

void Framework::writeExperiments() {
//    this->experiment->compileNumberOfActiveNodes();
//    this->experiment->compileNumberOfIterationInChildren();
}

int Framework::output() {
    vector<string> outputs;

    for (ActiveNode* activeNode : this->beva->currentActiveNodes) {
        unsigned beginRange = this->trie->getNode(activeNode->node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode->node).getEndRange();

        for (unsigned i = beginRange; i < endRange; i++) {
            outputs.push_back(this->records[i]);
        }
    }

//    for (const string& record : outputs) {
//        cout << record << "\n";
//    }

    return outputs.size();
}
