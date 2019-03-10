//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/utils.h"

Trie::Trie() {
    this->root = new Node();
}

void Trie::insert(string& record, int recordId) {
    Node* root = this->root;

    for (char i : record) {
        if ((int) i == -61) continue;
        else if ((int) i < 0 || (int) i >= CHAR_SIZE) {
            i = utils::convertSpecialCharToSimpleChar(i);
        }

        if (root->children[i] == nullptr) {
            root->children[i] = new Node(i);
        }
        root->children[i]->recordsId.push_back(recordId);
        root = root->children[i];
    }

    root->isEndOfWord = true;
}
