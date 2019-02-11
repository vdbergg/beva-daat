#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include "Trie.h"
#include "EditDistance.h"
#include "EditVectorAutomaton.h"

using namespace std;

vector<string> records;
vector<string> queries;

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

void printOutput(vector<ActiveNode*> currentActiveNodes) {
    for (ActiveNode* activeNode : currentActiveNodes) {
        vector<int> recordsId = activeNode->node->recordsId;
        for (int recordId : recordsId) {
            cout << records.at(recordId) << "\n";
        }
    }
}

int main() {
    string datasetFile = "/home/berg/Mestrado/workspace/beva/dataset/aol/aol.txt";
    string queryFile = "/home/berg/Mestrado/workspace/beva/dataset/aol/aol_queries/q7.txt";

    cout << "indexing... \n";
    auto start = chrono::high_resolution_clock::now();
    readData(datasetFile, records);
    readData(queryFile, queries);

    Trie* trie = new Trie();

    string query = "cez";
    string queryRemaining = "ling heigh";

//    string keys[10] = {"abelha", "abacaxi", "abacate", "abobora", "arvore", "arco", "ave", "agil", "atrevido", "atraente"};

    int recordId = 0;
    for (string record: records) {
        trie->insert(record, recordId);
        recordId++;
    }

    auto done = chrono::high_resolution_clock::now();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";

    int count = 0;
    int editDistanceThreshold = 1;

//    EditVectorAutomaton* editVectorAutomaton = new EditVectorAutomaton(editDistanceThreshold);
//    editVectorAutomaton->buildAutomaton();

    while (query.length() <= 13) {
        cout << "Query: " + query + "\n";
//        string temp;
//        cin >> temp;
//        query += temp;

        start = chrono::high_resolution_clock::now();
        vector<ActiveNode*> activeNodes = trie->autocomplete(query, editDistanceThreshold);
        printOutput(activeNodes);
        done = chrono::high_resolution_clock::now();

        cout << "<<<Process time: " << chrono::duration_cast<chrono::microseconds>(done - start).count() << " us>>>\n\n";
        this_thread::sleep_for(chrono::seconds(1));

        query += queryRemaining[count];
        query += queryRemaining[count + 1];
        count += 2;
    }

    return 0;
}
