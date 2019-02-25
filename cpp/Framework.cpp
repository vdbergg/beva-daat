//
// Created by vdberg on 12/02/19.
//

#include "../header/Framework.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include "../header/Trie.h"
#include "../header/C.h"

using namespace std;

Framework::Framework(int editDistanceThreshold, int dataset) {
    this->trie = nullptr;
    this->editDistanceThreshold = editDistanceThreshold;
    this->dataset = dataset;

    index();
}

Framework::~Framework() {

}

void readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        for (auto i = 0; i < str.length(); i++) {
            str[i] = tolower(str[i]);
        }
        recs.push_back(str);
    }
}

void Framework::index() {
    cout << "indexing... \n";
    auto start = chrono::high_resolution_clock::now();
    
    string datasetFile = "/datasets/autocompletion/";
    string queryFile = "/datasets/autocompletion/";

    switch (this->dataset) {
        case C::AOL:
            datasetFile += "aol/aol.txt";
            queryFile += "aol/q13_10.txt";
            break;
        case C::MEDLINE:
            datasetFile += "medline/medline.txt";
            queryFile += "medline/q13_10.txt";
            break;
        case C::USADDR:
            datasetFile += "usaddr/usaddr.txt";
            queryFile += "usaddr/q13_10.txt";
            break;
        default:
            datasetFile += "aol/aol.txt";
            queryFile += "aol/q13_10.txt";
            break;
    }

    readData(datasetFile, this->records);
    readData(queryFile, this->queries);

   this->trie = new Trie();

    int recordId = 0;
    for (string record: this->records) {
        this->trie->insert(record, recordId);
        recordId++;
    }

    this->beva = new Beva(this->trie, this->editDistanceThreshold);

    auto done = chrono::high_resolution_clock::now();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}

void Framework::process(string query, int algorithm, int queryLength) {
    cout << "Query: " + query + "\n";

    auto start = chrono::high_resolution_clock::now();

    switch (algorithm) {
        case C::BEVA:
            this->activeNodes = this->beva->process(query, this->activeNodes);
            break;
        default:
            this->activeNodes = this->beva->process(query, this->activeNodes);
            break;
    }
    output();

    auto done = chrono::high_resolution_clock::now();

    if (query.length() == queryLength) {
        this->activeNodes.clear(); // Clean the active nodes for next query
        this->beva->reset(this->trie); // Reset the information from previous query
    }

    cout << "<<<Process time: " << chrono::duration_cast<chrono::microseconds>(done - start).count() << " us>>>\n\n";
    this_thread::sleep_for(chrono::seconds(1));
}

void Framework::output() {
    for (ActiveNode* activeNode : this->activeNodes) {
        vector<int> recordsId = activeNode->node->recordsId;
        for (int recordId : recordsId) {
            cout << this->records.at(recordId) << "\n";
        }
    }
}
