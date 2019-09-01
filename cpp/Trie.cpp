//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/utils.h"
#include "../header/Experiment.h"

Trie::Trie(int datasetSize, Experiment* experiment) {
    this->root = new Node();
    this->root->beginRange = 0;
    this->root->endRange = datasetSize;
    this->experiment = experiment;
    this->experiment->incrementNumberOfNodes();
}

void Trie::append(const string& rec, const int recordId) {
    Node* node = this->root;
    int currentIndexLevel = 0;
    for (char ch : rec) {
        if ((int) ch == -61) continue;
        else if ((int) ch < 0 || (int) ch >= CHAR_SIZE) {
            ch = utils::convertSpecialCharToSimpleChar(ch);
        }

        node = this->insert(ch, node);

        currentIndexLevel++;

        if (node->beginRange == -1) {
            node->beginRange = recordId;
        }
        node->endRange = recordId + 1;
    }
    this->experiment->proportionOfBranchingSizeInBEVA2Level(currentIndexLevel);
}

Node* Trie::insert(char ch, Node* node) {
    auto vit = node->children.begin();
    for (; vit != node->children.end(); vit++) {
        if ((*vit)->value == ch) break;
    }

    if (vit == node->children.end()) {
        Node* newNode = new Node(ch);
        this->experiment->incrementNumberOfNodes();
        node->children.push_back(newNode);
        return newNode;
    }
    return *vit;
}
