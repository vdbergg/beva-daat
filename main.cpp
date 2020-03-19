#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "header/Framework.h"

using namespace std;

unordered_map<string,string> config;
void loadConfig();

int main(int argc, char** argv) {

    loadConfig();

    string q, query, queryRemaining;
    Framework* framework = new Framework(config);
    int indexMin = stoi(config["qry_number_start"]);
    int indexMax =  stoi(config["qry_number_end"]);
    cout << "processing...\n";

    for (int i = indexMin; i < indexMax; ++i) {
        q = framework->queries[i];
        query = "";
        queryRemaining = q.substr(0);

        int count = 0;

        while (query.length() <= q.length()) {
            framework->process(query, (int) q.length(), i);

            query += queryRemaining[count];
            count++;
//            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    delete framework;
    return 0;
}

void loadConfig() {

    std::ifstream is_file("/home/berg/workspace/beva/path.cfg");
    std::string line;

    while( std::getline(is_file, line) )
    {
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            std::string value;
            if( std::getline(is_line, value) )
                config[key] = value;
        }
    }
}