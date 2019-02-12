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
#include "EditDistance.h"
#include "EditVectorAutomaton.h"

using namespace std;

Framework::Framework() {
    this->trie = nullptr;

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
    string datasetFile = "/home/vdberg/Mestrado/workspace/beva/dataset/aol/aol.txt";
    string queryFile = "/home/vdberg/Mestrado/workspace/beva/dataset/aol/aol_queries/q7.txt";

    cout << "indexing... \n";
    auto start = chrono::high_resolution_clock::now();
    readData(datasetFile, this->records);
    readData(queryFile, this->queries);

   this->trie = new Trie();

    int recordId = 0;
    for (string record: this->records) {
        trie->insert(record, recordId);
        recordId++;
    }

    auto done = chrono::high_resolution_clock::now();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}

void Framework::process(string query, int editDistanceThreshold) {
    cout << "Query: " + query + "\n";

    auto start = chrono::high_resolution_clock::now();
    this->activeNodes = this->trie->autocomplete(query, editDistanceThreshold, this->activeNodes);
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
