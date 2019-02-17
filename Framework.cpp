//
// Created by vdberg on 12/02/19.
//

#include "Framework.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include "Trie.h"
#include "C.h"

using namespace std;

Framework::Framework(int editDistanceThreshold) {
    this->trie = nullptr;
    this->editDistanceThreshold = editDistanceThreshold;

    index();
}

Framework::~Framework() {

}

void readData(string& filename, vector<string>& recs) {
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
    string datasetFile = "/home/berg/Mestrado/workspace/beva/dataset/aol/aol.txt";
//    string queryFile = "/home/berg/Mestrado/workspace/beva/dataset/aol/aol_queries/q7.txt";

    cout << "indexing... \n";
    auto start = chrono::high_resolution_clock::now();
    readData(datasetFile, this->records);
//    readData(queryFile, this->queries);

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

void Framework::process(string query, int algorithm) {
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
