#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "header/Framework.h"
#include "header/C.h"

using namespace std;

map<string,string> config;
void loadConfig();

int main(int argc, char** argv) {

    loadConfig();

    string q, query, queryRemaining;
    Framework* framework = new Framework(config);
    int indexMin, indexMax;
    int algorithmType = 0; // = -> BEVA
    int qryNumber = stoi(config["qry_number"]);

    if(qryNumber == -1){
        indexMin = 0;
        indexMax = framework->queries.size();
    }
    else{
        indexMin = qryNumber;
        indexMax = qryNumber + 1;
    }

    for (int i = indexMin; i < indexMax; ++i) {
        q = framework->queries[i];
        query = q.substr(0, 1);
        queryRemaining = q.substr(1);

        int count = 0;

        while (query.length() <= q.length()) {
            framework->process(query, algorithmType, (int) q.length());

            query += queryRemaining[count];
            count++;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    delete framework;
    return 0;
}

void loadConfig() {

    std::ifstream is_file("path.cfg");
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