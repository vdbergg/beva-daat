//
// Created by vdberg on 12/02/19.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/Framework.h"
#include "../header/utils.h"
#include "../header/Experiment.h"

using namespace std;

Framework::Framework(map<string,string> config) {
    this->trie = nullptr;
    this->editDistanceThreshold = stoi(config["edit_distance"]);
    this->dataset = stoi(config["dataset"]);
    this->experiment = new Experiment(config, editDistanceThreshold);

    index(config);
}

Framework::~Framework() {
    cout << "deleting framework" << endl;
    delete this->beva;
    delete this->trie;
}

void readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        for (auto i = 0; i < str.length(); i++) {
            str[i] = tolower(str[i]);
        }
        str = utils::normalize(str);
        recs.push_back(str);
    }
}

void Framework::index(map<string,string> config) {
    cout << "indexing... \n";
    string sizeSufix = "";
    switch (stoi(config["size_type"])) {
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
    this->experiment->initIndexingTime();
    
    string datasetFile = config["dataset_basepath"];
    string queryFile = config["query_basepath"];

    switch (this->dataset) {
        case C::AOL:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q13_10.txt";
            break;
        case C::MEDLINE:
            datasetFile += "medline/medline" + sizeSufix + ".txt";
            queryFile += "medline/q13_10.txt";
            break;
        case C::USADDR:
            datasetFile += "usaddr/usaddr" + sizeSufix + ".txt";
            queryFile += "usaddr/q13_10.txt";
            break;
        default:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q13_10.txt";
            break;
    }

    readData(datasetFile, this->records);
    sort(this->records.begin(), this->records.end());
    readData(queryFile, this->queries);

    this->trie = new Trie(this->records.size());

    int recordId = 0;
    for (string& record : this->records) {
        this->trie->append(record, recordId);
        recordId++;
    }

    this->beva = new Beva(this->trie, this->editDistanceThreshold);

    auto done = chrono::high_resolution_clock::now();
    this->experiment->endIndexingTime();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}

void Framework::process(string query, int algorithm, int queryLength, int countQueryExpected,
        int currentCountQuery) {
    if (query.empty()) {
        this->activeNodes.push_back(new ActiveNode(this->trie->root, ""));
        return;
    }

    cout << "Query: " + query + "\n";

    query = utils::normalize(query);

    auto start = chrono::high_resolution_clock::now();
    this->experiment->initQueryProcessingTime();

    switch (algorithm) {
        case C::BEVA:
            this->activeNodes = this->beva->process(query, this->activeNodes);
            break;
        default:
            this->activeNodes = this->beva->process(query, this->activeNodes);
            break;
    }

    auto done = chrono::high_resolution_clock::now();
    this->experiment->endQueryProcessingTime(query.length());

//    output();

    if (currentCountQuery == countQueryExpected && query.length() == queryLength) {
        this->experiment->compileQueryProcessingTimes(countQueryExpected);
    }

    if (query.length() == queryLength) {
        this->activeNodes.clear(); // Clean the active nodes for next query
        this->beva->reset(this->trie); // Reset the information from previous query
    }

    cout << "<<<Process time: " << chrono::duration_cast<chrono::microseconds>(done - start).count() << " us>>>\n\n";
}

void Framework::output() {
    int count = 0;
    for (ActiveNode* activeNode : this->activeNodes) {
        int beginRange = activeNode->node->beginRange;
        int endRange = activeNode->node->endRange;

        if (beginRange != -1 && endRange != -1) {
            vector<string> results;
            if (endRange - beginRange != this->records.size()) {
                vector<string> recs(this->records.begin() + beginRange, this->records.begin() + endRange);
                results = recs;
            } else {
                results = this->records;
            }
            count += results.size();
            for (const string& record : results) {
                cout << record << "\n";
            }
        }
    }
    cout << "Results length: " + to_string(count) << "\n";
}
