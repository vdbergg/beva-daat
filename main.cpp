#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "header/Framework.h"
#include "header/Directives.h"
#include "header/crow_all.h"

using namespace std;

unordered_map<string, string> config;
void loadConfig();
int queryId = 0;
Framework* framework;
const int MAX_QUERY_CHARACTER = 17;


int main(int argc, char** argv) {
    loadConfig();

    framework = new Framework(config);
    int indexMin = stoi(config["qry_number_start"]);
    int indexMax =  stoi(config["qry_number_end"]);

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        indexMax = 100;
    #endif

    if (config["load_test"] == "0") {
        cout << "processing...\n";

        for (int i = indexMin; i < indexMax; ++i) {
            vector<ActiveNode> currentActiveNodes;
            vector<ActiveNode> oldActiveNodes;
            unsigned bitmaps[CHAR_SIZE];
            for (auto & bitmap : bitmaps) bitmap = framework->beva->bitmapZero;

            for (int currentPrefixQuery = 1; currentPrefixQuery <= MAX_QUERY_CHARACTER; currentPrefixQuery++) {
                swap(oldActiveNodes, currentActiveNodes);
                currentActiveNodes.clear();
                framework->process(framework->queries[i], currentPrefixQuery, i, oldActiveNodes,
                        currentActiveNodes, bitmaps);
                oldActiveNodes.clear();
            }
        }
    } else {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/")
                .name("hello")
                        ([]{
                            return "Hello World!";
                        });

        CROW_ROUTE(app, "/about")
                ([](){
                    return "About Crow example.";
                });

        CROW_ROUTE(app, "/autocomplete")
                ([](const crow::request& req) {
                    std::ostringstream os;
                    vector<char *> results;

                    os << "Params: " << req.url_params << "\n\n";

//                if (req.url_params.get("load_config") != nullptr) {
//                    string load_config = boost::lexical_cast<string>(req.url_params.get("load_config"));
//                    os << "The value of 'load_config' is " << load_config << '\n';
//                    if (load_config == "true") {
//                        loadConfig();
//                    }
//                }

                    if (req.url_params.get("query") != nullptr) {
                        string query = boost::lexical_cast<string>(req.url_params.get("query"));
                        os << "The value of 'query' is " <<  query << '\n';

                        vector<ActiveNode> currentActiveNodes;
                        vector<ActiveNode> oldActiveNodes;
                        unsigned bitmaps[CHAR_SIZE];
                        for (auto & bitmap : bitmaps) bitmap = framework->beva->bitmapZero;

                        for (int currentPrefixQuery = 1; currentPrefixQuery <= query.size(); currentPrefixQuery++) {
                            swap(oldActiveNodes, currentActiveNodes);
                            currentActiveNodes.clear();
                            framework->process(query, currentPrefixQuery, queryId, oldActiveNodes,
                                    currentActiveNodes, bitmaps);

                            #ifdef BEVA_IS_COLLECT_TIME_H
                                if (config["load_test"] == "1" && currentPrefixQuery == 17) {
                                    results = framework->output(currentActiveNodes);
                                }
                            #endif
                            oldActiveNodes.clear();
                        }
                    }

                    queryId++;

                    crow::json::wvalue response;
                    response["results"] = results;
                    return response;
//                return crow::response{os.str()};
                });

        // ignore all log
        crow::logger::setLogLevel(crow::LogLevel::Debug);
        //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

        app.port(18080)
//            .concurrency(10)
                .multithreaded()
                .run();
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
