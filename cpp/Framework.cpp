//
// Created by vdberg on 12/02/19.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <sys/stat.h>
#include "../header/Trie.h"
#include "../header/C.h"
#include "../header/Framework.h"
#include "../header/utils.h"
#include "../header/Experiment.h"
#include "../header/Directives.h"

using namespace std;

Framework::Framework(unordered_map<string, string> config) {
    this->trie = nullptr;
    this->editDistanceThreshold = stoi(config["edit_distance"]);
    this->dataset = stoi(config["dataset"]);
    this->experiment = new Experiment(config, editDistanceThreshold);
    this->config = config;

    index();
}

Framework::~Framework() {
    cout << "deleting framework" << endl;
    delete this->beva;
    delete this->trie;
    delete this->experiment;
}

unsigned long getFileSize(string filename) {
    FILE *fp=fopen(filename.c_str(),"r");

    struct stat buf;
    fstat(fileno(fp), &buf);
    fclose(fp);
    return buf.st_size;
}

void Framework::readData(string& filename, vector<StaticString>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);

    unsigned long fileSize = getFileSize(filename);
//    cout << "Tamanho do Arquivo:" << fileSize << endl;
    char *tmpPtr = (char*) malloc(sizeof(char)*fileSize);
    StaticString::setDataBaseMemory(tmpPtr,fileSize);
    while (getline(input, str)) {
        for (char &c : str) {
            if ((int) c == -61) continue;
            else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
                c = utils::convertSpecialCharToSimpleChar(c);
            }
            c = tolower(c);
        }
        if (!str.empty()) recs.push_back(StaticString(str));
    }
}

void Framework::readData(string& filename, vector<string>& recs) {
    cout << "reading dataset " << filename << endl;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        for (char &c : str) {
            if ((int) c == -61) continue;
            else if ((int) c < 0 || (int) c >= CHAR_SIZE) {
                c = utils::convertSpecialCharToSimpleChar(c);
            }
            c = tolower(c);
        }
        if (!str.empty()) recs.push_back(str);
    }
}

void Framework::index(){
    cout << "indexing... \n";
    string sizeSufix = "";
    switch (stoi(this->config["size_type"])) {
        case 0:
            sizeSufix = "_25";
            break;
        case 1:
            sizeSufix = "_50";
            break;
        case 2:
            sizeSufix = "_75";
            break;
        case 3:
            sizeSufix = "";
            break;
        default:
            sizeSufix = "_60";
    }

    auto start = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->initIndexingTime();
    #endif
    
    string datasetFile = this->config["dataset_basepath"];
    string queryFile = this->config["query_basepath"];

    int queriesSize = stoi(config["queries_size"]);
    string datasetSuffix = queriesSize == 10 ? "_10" : "";
    string tau = to_string(this->editDistanceThreshold);

    switch (this->dataset) {
        case C::AOL:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::MEDLINE:
            datasetFile += "medline/medline" + sizeSufix + ".txt";
            queryFile += "medline/q13" + datasetSuffix + ".txt";
            break;
        case C::USADDR:
            datasetFile += "usaddr/usaddr" + sizeSufix + ".txt";
            queryFile += "usaddr/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::MEDLINE19:
            datasetFile += "medline19/medline19" + sizeSufix + ".txt";
            queryFile += "medline19/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::DBLP:
            datasetFile += "dblp/dblp" + sizeSufix + ".txt";
            queryFile += "dblp/q17_" + tau + datasetSuffix + ".txt";
	    break;
        case C::UMBC:
            datasetFile += "umbc/umbc" + sizeSufix + ".txt";
            queryFile += "umbc/q17_" + tau + datasetSuffix + ".txt";
            break;
        case C::JUSBRASIL:
            datasetFile += "jusbrasil/jusbrasil" + sizeSufix + ".txt";
            queryFile += "jusbrasil/q.txt";
            break;
        default:
            datasetFile += "aol/aol" + sizeSufix + ".txt";
            queryFile += "aol/q17_" + tau + datasetSuffix + ".txt";
            break;
    }

    readData(datasetFile, records);
    //    sort(this->records.begin(), this->records.end());
    readData(queryFile, this->queries);

    this->trie = new Trie(this->experiment);
    this->trie->buildDaatIndex();
    this->trie->shrinkToFit();

    this->beva = new Beva(this->trie, this->experiment, this->editDistanceThreshold);

    auto done = chrono::high_resolution_clock::now();

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        this->experiment->getMemoryUsedInIndexing();
    #else
        this->experiment->endIndexingTime();
        this->experiment->compileProportionOfBranchingSizeInBEVA2Level();
        this->experiment->compileNumberOfNodes();
    #endif
    cout << "<<<Index time: "<< chrono::duration_cast<chrono::milliseconds>(done - start).count() << " ms>>>\n";
}


void Framework::process(string query, int prefixQueryLength, int currentCountQuery,
        vector<ActiveNode>& oldActiveNodes, vector<ActiveNode>& currentActiveNodes, unsigned (&bitmaps)[CHAR_SIZE]) {
    if (query.empty()) return;

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->initQueryProcessingTime();
    #endif

    this->beva->process(query[prefixQueryLength - 1], prefixQueryLength, oldActiveNodes, currentActiveNodes,
            bitmaps);

    #ifdef BEVA_IS_COLLECT_TIME_H
        this->experiment->endQueryProcessingTime(currentActiveNodes.size(), prefixQueryLength);

        if (config["load_test"] == "0") {
//            if (prefixQueryLength == 5 || prefixQueryLength == 9 || prefixQueryLength == 13
//            || prefixQueryLength == 17) {
            if (prefixQueryLength == query.size()) {
                this->experiment->initQueryFetchingTime();
                vector<char *> results = output(currentActiveNodes);
                this->experiment->endQueryFetchingTime(prefixQueryLength, results.size());
            }
        }
    #endif

    currentActiveNodes.shrink_to_fit();
    if (query.length() == prefixQueryLength) {
        #ifdef BEVA_IS_COLLECT_MEMORY_H
            this->experiment->getMemoryUsedInProcessing();
        #else
            this->experiment->compileQueryProcessingTimes(currentCountQuery);
            string currentQuery = query.substr(0, prefixQueryLength);
            this->experiment->saveQueryProcessingTime(currentQuery, currentCountQuery);
        #endif
    }
}

void Framework::writeExperiments() {
    #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
        this->experiment->compileNumberOfActiveNodes();
        this->experiment->compileNumberOfIterationInChildren();
    #endif
}

vector<char *> Framework::output(vector<ActiveNode>& currentActiveNodes) {
    vector<char *> outputs;
    string tmp;

    for (ActiveNode activeNode : currentActiveNodes) {
        unsigned beginRange = this->trie->getNode(activeNode.node).getBeginRange();
        unsigned endRange = this->trie->getNode(activeNode.node).getEndRange();

        for (unsigned i = beginRange; i < endRange; i++) {
            outputs.push_back(records[i].c_str());
        }
    }

    //    for (const string& record : outputs) {
    //     cout << record << "\n";
    // }

    return outputs;
}
