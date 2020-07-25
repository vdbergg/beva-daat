#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "header/Framework.h"
#include "header/Directives.h"

using namespace std;

unordered_map<string,string> config;
void loadConfig();

int main(int argc, char** argv) {
    loadConfig();

    Framework* framework = new Framework(config);
    int indexMin = stoi(config["qry_number_start"]);
    int indexMax =  stoi(config["qry_number_end"]);

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        indexMax = 100;
    #endif

    const int MAX_QUERY_CHARACTER = 17;

    cout << "processing...\n";

    for (int i = indexMin; i < indexMax; ++i) {
        for (int currentPrefixQuery = 1; currentPrefixQuery <= MAX_QUERY_CHARACTER; currentPrefixQuery++) {
            framework->process(framework->queries[i], currentPrefixQuery, i);
        }
    }

    #ifdef BEVA_IS_COLLECT_COUNT_OPERATIONS_H
    framework->writeExperiments();
    #endif

    delete framework;
    return 0;
}

void loadConfig() {

    std::ifstream is_file("./path.cfg");
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
