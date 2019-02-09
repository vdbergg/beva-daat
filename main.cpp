#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include "Trie.h"
#include "EditDistance.h"

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

    for (string key: records) {
        trie->insert(key);
    }

    auto done = chrono::high_resolution_clock::now();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";

    int count = 0;
    int editDistanceThreshold = 2;

    while (query.length() <= 13) {
        cout << "Query: " + query + "\n";
//        string temp;
//        cin >> temp;
//        query += temp;

        start = chrono::high_resolution_clock::now();
        trie->autocomplete(query, editDistanceThreshold);
        done = chrono::high_resolution_clock::now();

        cout << "<<<Process time: " << chrono::duration_cast<chrono::microseconds>(done - start).count() << " us>>>\n\n";
        this_thread::sleep_for(chrono::seconds(1));

        query += queryRemaining[count];
        query += queryRemaining[count + 1];
        count += 2;
    }

    return 0;
}
