//
// Created by berg on 13/01/19.
//

#include "../header/Trie.h"
#include "../header/utils.h"
#include "../header/Experiment.h"

Trie::Trie(int datasetSize, Experiment* experiment) {
    this->root = new Node();
    this->root->setBeginRange(0);
    this->root->setEndRange(datasetSize);
    this->experiment = experiment;
    this->experiment->incrementNumberOfNodes();
}

void Trie::append(const string& rec, const int recordId) {
    Node* node = this->root;
    int currentIndexLevel = 0;
    for (unsigned char ch : rec) {
        if ( ch == 195) continue;
        else if (ch >= CHAR_SIZE) {
            ch = utils::convertSpecialCharToSimpleChar(ch);
        }

        node = this->insert((char)ch, recordId, node);

        currentIndexLevel++;

        node->setEndRange(recordId + 1);
    }
    node->isEndOfWord = true;
    this->experiment->proportionOfBranchingSizeInBEVA2Level(currentIndexLevel);
}

Node* Trie::insert(char ch, int recordId, Node* node) {
  ShortVector<Node*>::iterator vit = node->children.begin();
    for (; vit != node->children.end(); vit++) {
      if ((*vit)->getValue() == ch) break;
    }

    if (vit == node->children.end()) {
        Node* newNode = new Node(ch);
        newNode->setBeginRange(recordId);
        node->children.push_back(newNode);
        this->experiment->incrementNumberOfNodes();
        return newNode;
    }
    return *vit;
}
