#pragma once

#include "request.h"
#include <httplib.h>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

enum class Commands {
    EFFORT,
    MODEL,
    CLEAR,
    EXIT,
    NEW,
    GOAL,
    HELP,
    PLAN,
    USAGE,

    None
};

class RequestManager {

    private:
        std::string model;
        std::string endpoint;
        httplib::Client client;

        long long totalTokenCount = 0;

        std::string getUserPrompt();

        Commands getCommand(std::string message);

    public:
        RequestManager(std::string modelName, std::string endpointURL);
        RequestManager(std::string endpointURL);

        void setModel(std::string modelName);
        void chooseModel();

        std::vector<std::string> getModelList();

        void message();

};

class ContextManager {
    int maxContext;
};
