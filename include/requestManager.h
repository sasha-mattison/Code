#pragma once

#include "request.h"
#include <httplib.h>
#include <iostream>

class RequestManager {

    private:
        std::string model;
        std::string endpoint;
        httplib::Client client;

        std::string getUserPrompt();

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
