#include <iostream>
#include <vector>
#include <fstream>
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
    string datasetFile = "/home/vdberg/Mestrado/beva/dataset/aol/aol.txt";
    string queryFile = "/home/vdberg/Mestrado/beva/dataset/aol/aol_queries/q7.txt";

//    readData(datasetFile, records);
//    readData(queryFile, queries);

    Trie* trie = new Trie();

    string query; // = "battleships us 2";

    string keys[10] = {"abelha", "abacaxi", "abacate", "abobora", "arvore", "arco", "ave", "agil", "atrevido", "atraente"};

    for (string key: keys) {
        trie->insert(key);
    }

    cout << "Insert your query:";
    cin >> query;

    trie->autocomplete(query, 3);

    return 0;
}
