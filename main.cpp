#include <iostream>
#include "Trie.h"

using namespace std;

int main() {
    Trie* trie = new Trie();

    string query;

    string keys[10] = {"abelha", "abacaxi", "abacate", "abobora", "arvore", "arco", "ave", "agil", "atrevido", "atraente"};

    for (string key: keys) {
        trie->insert(key);
    }

    cout << "Insert your query:";
    cin >> query;

    trie->autocomplete(query);

    return 0;
}