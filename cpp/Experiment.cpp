//
// Created by berg on 12/07/19.
//

#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "../header/Experiment.h"
#include "../header/utils.h"
#include "../header/QueryResult.h"

using namespace std;

const int MAX_QUERY_CHARACTER = 13;

string getFilename(map<string, string> config, string filename, int editDistanceThreshold) {
    string name = config["experiments_basepath"] + filename;
    name += "_data_set_" + config["dataset"] + "_size_type_" + config["size_type"] +
                "_tau_" + to_string(editDistanceThreshold) + "_alg_" + config["alg"] + ".txt";

    return name;
}

Experiment::Experiment(map<string, string> config, int editDistanceThreshold) {
    this->config = std::move(config);
    this->editDistanceThreshold = editDistanceThreshold;

    for (int i = 0; i < MAX_QUERY_CHARACTER; i++) {
        this->processingTimes.push_back(0);
        this->activeNodesSizes.push_back(0);
        this->currentActiveNodesSize.push_back(0);
        this->currentQueryProcessingTime.push_back(0);
    }

    this->recoveryMode = this->config["recovery_mode"] == "0" ? false : true;

    if (this->recoveryMode) {
        string filename = getFilename(this->config, "query_processing_time", this->editDistanceThreshold);
        this->readQueryProcessingTime(filename);

        filename = getFilename(this->config, "top_queries_with_short_processing_time", this->editDistanceThreshold);
        this->readQueriesProcessingTime(filename);

        filename = getFilename(this->config, "top_queries_with_long_processing_time", this->editDistanceThreshold);
        this->readQueriesProcessingTime(filename);
    }
}

void Experiment::readQueriesProcessingTime(string& filename) {
    int count = 0, countQuery = 0;
    long accumulatedProcessingTime = 0;

    string query;
    vector<long> cQueryProcessingTime;
    vector<long> cActiveNodesSize;

    for (int i = 0; i < MAX_QUERY_CHARACTER; i++) {
        cQueryProcessingTime.push_back(0);
        cActiveNodesSize.push_back(0);
    }

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        vector<string> tokens = utils::split(str, '\t');
        int tokensSize = tokens.size();

        if (tokensSize == 2) {
            if (countQuery > 0) {
                auto *queryResult = new QueryResult(accumulatedProcessingTime, cQueryProcessingTime,
                        cActiveNodesSize);
                this->processingTimeByQuery.emplace_back(query, queryResult);
                for (int i = 0; i < MAX_QUERY_CHARACTER; i++) {
                    cActiveNodesSize[i] = 0;
                    cQueryProcessingTime[i] = 0;
                }
            }

            query = tokens[0];
            accumulatedProcessingTime = stol(tokens[1]);
            countQuery++;
            count = 0;
        } else if (tokensSize == 4) {
            cQueryProcessingTime[count] = stol(tokens[1]);
            cActiveNodesSize[count] = stol(tokens[3]);
            count++;
        }
    }
}

void Experiment::readQueryProcessingTime(string& filename) {
    int count = 0, countLine = 0;
    int queriesProcessed = 1;

    string str;
    ifstream input(filename, ios::in);
    while (getline(input, str)) {
        vector<string> tokens = utils::split(str, '\t');
        int tokensSize = tokens.size();

        if (tokensSize == 1) {
            queriesProcessed = stoi(tokens[0]);
        } else if (tokensSize == 4 && countLine > 1) {
            this->processingTimes[count] = stol(tokens[1]) * (queriesProcessed + 1);
            this->activeNodesSizes[count] = stof(tokens[3]) * (queriesProcessed + 1);
            count++;
        }
        countLine++;
    }
}

void Experiment::writeFile(const string& name, const string& value) {
    ofstream myfile;
    string newName = config["experiments_basepath"] + name;
    newName += "_data_set_" + config["dataset"] + "_size_type_" + config["size_type"] +
            "_tau_" + to_string(this->editDistanceThreshold) + "_alg_" + config["alg"] + ".txt";
    cout << newName << "\n";
    myfile.open(newName);

    if (myfile.is_open()) {
        myfile << value;
        myfile.close();
    } else {
        cout << "Unable to open file.\n";
    }
}

void Experiment::initIndexingTime() {
    this->startIndexingTime = chrono::high_resolution_clock::now();
}

void Experiment::endIndexingTime() {
    this->finishIndexingTime = chrono::high_resolution_clock::now();

    long result = chrono::duration_cast<chrono::microseconds>(
            this->finishIndexingTime - this->startIndexingTime
            ).count();


    writeFile("indexing_time", "indexing_time\n" + to_string(result) + "\n");
}

void Experiment::initQueryProcessingTime() {
    this->startQueryProcessingTime = chrono::high_resolution_clock::now();
}

void Experiment::endQueryProcessingTime(long activeNodesSize, string &query, int queryId) {
    this->finishQueryProcessingTime = chrono::high_resolution_clock::now();

    long currentQueryLength = query.size();

    long result = chrono::duration_cast<chrono::microseconds>(
            this->finishQueryProcessingTime - this->startQueryProcessingTime
    ).count();

    this->currentQueryProcessingTime[currentQueryLength - 1] = result;
    this->currentActiveNodesSize[currentQueryLength - 1] = activeNodesSize;
    this->processingTimes[currentQueryLength - 1] += result;
    this->activeNodesSizes[currentQueryLength - 1] += (float) activeNodesSize;

    if (currentQueryLength == MAX_QUERY_CHARACTER) {
        long accum = 0;
        for (long processingTime : this->currentQueryProcessingTime) {
            accum += processingTime;
        }
        auto* queryResult = new QueryResult(accum, this->currentQueryProcessingTime,
                this->currentActiveNodesSize);
        this->processingTimeByQuery.emplace_back(query, queryResult);

        this->compileQueryProcessingTimes(queryId);
        this->compileLongAndShortProcessingTimeQueries(queryId);
    }
}

void Experiment::compileQueryProcessingTimes(int queryId) {
    string value = to_string(queryId) + "\n";
    value += "query_size\tquery_processing_time\taccumulated_query_processing_time\tactive_nodes_size\n";
    int accum = 0;
    for (int i = 0; i < this->processingTimes.size(); i++) {
        long processingTime = this->processingTimes[i] / (queryId + 1);
        float activeNodesSize = this->activeNodesSizes[i] / (queryId + 1);
        stringstream stream;
        stream << std::fixed << std::setprecision(1) << activeNodesSize;
        accum += processingTime;
        value += to_string(i + 1) + "\t" + to_string(processingTime) +
                "\t" + to_string(accum) + "\t" + stream.str() + "\n";
    }

    writeFile("query_processing_time", value);
}

void Experiment::proportionOfBranchingSizeInBEVA2Level(int size) {
    if (this->branchSize.find(size) == this->branchSize.end() ) {
        this->branchSize[size] = 1;
    } else {
        this->branchSize[size]++;
    }
}

void Experiment::compileProportionOfBranchingSizeInBEVA2Level() {
    string value = "branch_size\tnumber_of_branches\n";
    for (map<int, int>::iterator it = this->branchSize.begin(); it != this->branchSize.end(); ++it) {
        value += to_string(it->first) + "\t" + to_string(it->second) + "\n";
    }
    writeFile("proportion_branch_size_BEVA", value);
}

void Experiment::incrementNumberOfNodes() {
    this->numberOfNodes++;
}

void Experiment::compileNumberOfNodes() {
    string value = "number_of_nodes\n";
    value += to_string(this->numberOfNodes) + "\n";
    writeFile("number_of_nodes_BEVA", value);
}

void Experiment::compileLongAndShortProcessingTimeQueries(int queryId) {
    sort(this->processingTimeByQuery.begin(), this->processingTimeByQuery.end(), utils::sortQueryProcessingTime);

    int top = 5;
    if (this->processingTimeByQuery.size() < top) {
        top = this->processingTimeByQuery.size();
    }

    string longProcessingTimeValue = to_string(queryId) + "\n";
    string shortProcessingTimeValue = to_string(queryId) + "\n";
    int n = this->processingTimeByQuery.size() - 1;

    for (int i = 0; i < top; i++) {
        auto* queryResultShort = this->processingTimeByQuery[i].second;
        shortProcessingTimeValue += this->processingTimeByQuery[i].first + "\t" +
                to_string(queryResultShort->queryProcessingTime) + "\n";
        vector<long>prefixProcessingTimeShort = queryResultShort->prefixProcessingTime;
        vector<long>prefixActiveNodeSizesShort = queryResultShort->prefixActiveNodeSizes;
        long accum = 0;

        for (int j = 0; j < prefixProcessingTimeShort.size(); j++) {
            accum += prefixProcessingTimeShort[j];
            shortProcessingTimeValue += to_string(j + 1) + "\t" + to_string(prefixProcessingTimeShort[j]) + "\t" +
                    to_string(accum) + "\t" + to_string(prefixActiveNodeSizesShort[j]) + "\n";
        }

        auto* queryResultLong = this->processingTimeByQuery[n - i].second;
        longProcessingTimeValue += this->processingTimeByQuery[n - i].first + "\t" +
                                    to_string(queryResultLong->queryProcessingTime) + "\n";
        vector<long>prefixProcessingTimeLong = queryResultLong->prefixProcessingTime;
        vector<long>prefixActiveNodeSizesLong = queryResultLong->prefixActiveNodeSizes;
        accum = 0;

        for (int j = 0; j < prefixProcessingTimeLong.size(); j++) {
            accum += prefixProcessingTimeLong[j];
            longProcessingTimeValue += to_string(j + 1) + "\t" + to_string(prefixProcessingTimeLong[j]) + "\t" +
                    to_string(accum) + "\t" + to_string(prefixActiveNodeSizesLong[j]) + "\n";
        }
    }
    writeFile("top_queries_with_short_processing_time", shortProcessingTimeValue);
    writeFile("top_queries_with_long_processing_time", longProcessingTimeValue);
}
