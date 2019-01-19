#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
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

    string query = "cezling";

//    string keys[10] = {"abelha", "abacaxi", "abacate", "abobora", "arvore", "arco", "ave", "agil", "atrevido", "atraente"};

    for (string key: records) {
        trie->insert(key);
    }

    auto done = chrono::high_resolution_clock::now();
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";

//    while (true) {
//        cout << "Insert your query: " + query;
//        string temp;
//        cin >> temp;
//        query += temp;

        start = chrono::high_resolution_clock::now();
        trie->autocomplete(query, 2);
        done = chrono::high_resolution_clock::now();

        cout << "<<<Process time: " << chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
//    }

    return 0;
}
