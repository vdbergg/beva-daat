//
// Created by berg on 12/07/19.
//

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <unistd.h>

#include "../header/Experiment.h"
#include "../header/utils.h"
#include "../header/QueryResult.h"

using namespace std;

const int MAX_QUERY_CHARACTER = 17;

string getFilename(map<string, string> config, const string& filename, int editDistanceThreshold) {
    string name = config["experiments_basepath"] + filename;
    name += "_data_set_" + config["dataset"] + "_size_type_" + config["size_type"] +
                "_tau_" + to_string(editDistanceThreshold) + "_alg_" + config["alg"] + ".txt";

    return name;
}

string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

Experiment::Experiment(map<string, string> config, int editDistanceThreshold) {
    this->config = std::move(config);
    this->editDistanceThreshold = editDistanceThreshold;

    for (int i = 0; i < MAX_QUERY_CHARACTER; i++) {
        this->processingTimes.push_back(0);
        this->activeNodesSizes.push_back(0);
        this->currentActiveNodesSize.push_back(0);
        this->currentQueryProcessingTime.push_back(0);
        this->currentResultsSize.push_back(0);
        this->memoryUsedInProcessing.push_back(0);
        this->fetchingTimes.push_back(0);
        this->resultsSize.push_back(0);
        this->currentQueryFetchingTime.push_back(0);
    }

    this->recoveryMode = !(this->config["recovery_mode"] == "0");

    if (this->recoveryMode) {
        string filename = getFilename(this->config, "query_processing_time", this->editDistanceThreshold);
        this->readQueryProcessingTime(filename);
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
        } else if (tokensSize == 6 && countLine > 1) {
            this->processingTimes[count] = stol(tokens[1]) * (queriesProcessed + 1);
            this->activeNodesSizes[count] = stof(tokens[5]) * (queriesProcessed + 1);
            this->fetchingTimes[count] = stol(tokens[3]) * (queriesProcessed + 1);
            this->resultsSize[count] = stol(tokens[4]) * (queriesProcessed + 1);
            count++;
        }
        countLine++;
    }
}

void Experiment::writeFile(const string& name, const string& value, bool writeInTheEnd) {
    ofstream myfile;
    string newName = config["experiments_basepath"] + name;
    newName += "_data_set_" + config["dataset"] + "_size_type_" + config["size_type"] +
            "_tau_" + to_string(this->editDistanceThreshold) + "_alg_" + config["alg"] + ".txt";

    if (writeInTheEnd) {
        myfile.open(newName, std::ios::app);
    } else {
        myfile.open(newName);
    }

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

    long result = chrono::duration_cast<chrono::nanoseconds>(
            this->finishIndexingTime - this->startIndexingTime
            ).count();


    writeFile("indexing_time", "indexing_time\n" + to_string(result) + "\n");
}

void Experiment::initQueryFetchingTime() {
    this->startQueryFetchingTime = chrono::high_resolution_clock::now();
}

void Experiment::endQueryFetchingTime(string &query, int queryId, int resultsSize) {
    this->finishQueryFetchingTime = chrono::high_resolution_clock::now();

    int currentQueryLength = query.size();

    long result = chrono::duration_cast<chrono::nanoseconds>(
            this->finishQueryFetchingTime - this->startQueryFetchingTime
    ).count();

    this->currentQueryFetchingTime[currentQueryLength - 1] = result;
    this->currentResultsSize[currentQueryLength - 1] = resultsSize;
    this->fetchingTimes[currentQueryLength - 1] += result;
    this->resultsSize[currentQueryLength - 1] += resultsSize;

    if (currentQueryLength == MAX_QUERY_CHARACTER) {
        this->compileQueryProcessingTimes(queryId);
        this->saveQueryProcessingTime(query, queryId);
    }
}

void Experiment::initQueryProcessingTime() {
    this->startQueryProcessingTime = chrono::high_resolution_clock::now();
}

void Experiment::endQueryProcessingTime(long activeNodesSize, string &query) {
    this->finishQueryProcessingTime = chrono::high_resolution_clock::now();

    long currentQueryLength = query.size();

    long result = chrono::duration_cast<chrono::nanoseconds>(
            this->finishQueryProcessingTime - this->startQueryProcessingTime
    ).count();

    this->currentQueryProcessingTime[currentQueryLength - 1] = result;
    this->currentActiveNodesSize[currentQueryLength - 1] = activeNodesSize;
    this->processingTimes[currentQueryLength - 1] += result;
    this->activeNodesSizes[currentQueryLength - 1] += (float) activeNodesSize;
}

void Experiment::saveQueryProcessingTime(string& query, int queryId) {
    string value = query + "\t" + to_string(queryId) + "\n";

    long accum = 0;

    for (int j = 0; j < this->currentQueryProcessingTime.size(); j++) {
        accum += this->currentQueryProcessingTime[j];
        value += to_string(j + 1) + "\t" + to_string(this->currentQueryProcessingTime[j]) + "\t" +
                                    to_string(accum) + "\t" + to_string(this->currentQueryFetchingTime[j]) + "\t" +
                                    to_string(this->currentResultsSize[j]) + "\t" +
                                    to_string(this->currentActiveNodesSize[j]) + "\n";

        this->currentQueryProcessingTime[j] = 0;
        this->currentActiveNodesSize[j] = 0;
        this->currentQueryFetchingTime[j] = 0;
        this->currentResultsSize[j] = 0;
    }

    writeFile("all_time_values", value, true);
}

void Experiment::compileQueryProcessingTimes(int queryId) {
    string value = to_string(queryId) + "\n";
    value += "query_size\tquery_processing_time\taccumulated_query_processing_time\tfetching_time\tresults_size\t"
             "active_nodes_size\n";

    int accum = 0;
    for (int i = 0; i < this->processingTimes.size(); i++) {
        long processingTime = this->processingTimes[i] / (queryId + 1);
        float activeNodesSize = this->activeNodesSizes[i] / (queryId + 1);
        long fetchingTime = this->fetchingTimes[i] / (queryId + 1);
        int _resultsSize = this->resultsSize[i] / (queryId + 1);
        stringstream stream;
        stream << std::fixed << std::setprecision(1) << activeNodesSize;
        accum += processingTime;
        value += to_string(i + 1) + "\t" + to_string(processingTime) +
                "\t" + to_string(accum) + "\t" + to_string(fetchingTime) +
                "\t" + to_string(_resultsSize) + "\t" + stream.str() + "\n";
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
    writeFile("proportion_branch_size", value);
}

void Experiment::incrementNumberOfNodes() {
    this->numberOfNodes++;
}

void Experiment::compileNumberOfNodes() {
    string value = "number_of_nodes\n";
    value += to_string(this->numberOfNodes) + "\n";
    writeFile("number_of_nodes", value);
}

void Experiment::getMemoryUsedInProcessing(int currentQueryLength) {
    pid_t pid = getpid();
    string cmd = "/bin/ps -p " + to_string(pid) + " -o size";
    string output = exec(cmd.c_str());

    vector<string> tokens = utils::split(output, '\n');

    float memoryUsed = stof(tokens[1]) / 1000;

    this->memoryUsedInProcessing[currentQueryLength - 1] = memoryUsed;

    if (currentQueryLength == MAX_QUERY_CHARACTER) {
       float avgMemoryUsed = 0;
       for (float memory : this->memoryUsedInProcessing) {
           avgMemoryUsed += memory;
       }
       avgMemoryUsed /= this->memoryUsedInProcessing.size();

       string value = to_string(avgMemoryUsed) + "\n";

       writeFile("memory_used_in_processing", value, true);

       for (int i = 0; i < this->memoryUsedInProcessing.size(); i++) {
           this->memoryUsedInProcessing[i] = 0;
       }
    }
}

void Experiment::getMemoryUsedInIndexing() {
    pid_t pid = getpid();
    string cmd = "/bin/ps -p " + to_string(pid) + " -o size";
    string output = exec(cmd.c_str());

    vector<string> tokens = utils::split(output, '\n');

    float memoryUsed = stof(tokens[1]) / 1000;

    string value = to_string(memoryUsed) + "\n";

    writeFile("memory_used_in_indexing", "memory_total_mb\tmemory_used_mb\n" + value);
}