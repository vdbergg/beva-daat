#include <iostream>
#include <string>
#include "header/Framework.h"
#include "header/Directives.h"
#include "header/crow_all.h"

using namespace std;

unordered_map<string, string> config;
void loadConfig();
Framework* framework;


void processingQueriesOutsideServer() {
    cout << "processing...\n";

    int indexMin = stoi(config["qry_number_start"]);
    int indexMax =  stoi(config["qry_number_end"]);

    #ifdef BEVA_IS_COLLECT_MEMORY_H
        indexMax = 100;
    #endif

    if (config["is_full_query_instrumentation"] == "0") {
        for (int i = indexMin; i < indexMax; ++i) {
            framework->processQuery(framework->queries[i], i);
        }
    } else {
        for (int i = indexMin; i < indexMax; ++i) {
            framework->processFullQuery(framework->queries[i], i);
        }
    }
}

void processingQueriesInServer() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
            .name("hello")
                    ([]{
                        return "Hello World!";
                    });

    CROW_ROUTE(app, "/about")
            ([](){
                return "This is a server";
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
                    results = framework->processFullQuery(query);
                }

                crow::json::wvalue response;
                response["results"] = results;
                cout << "<<<< Response length >>>> " + to_string(results.size()) << endl;

                return response;
//                return crow::response{os.str()};
            });

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::Debug);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
//      .concurrency(10)
        .multithreaded()
        .run();
}


int main(int argc, char** argv) {
    loadConfig();

    framework = new Framework(config);

    if (config["is_server"] == "0") {
        processingQueriesOutsideServer();
    } else {
        processingQueriesInServer();
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

        if (std::getline(is_line, key, '=')) {
            std::string value;

            if (std::getline(is_line, value)) {
                config[key] = value;
            }
        }
    }
}
