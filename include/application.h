#pragma once

#include "requestManager.h"

class Application {

    private:

    std::string model;
    std::string endpoint = "http://localhost:11434";
    RequestManager reqM;

    std::string chooseModel();

    public:
        Application();
        void run();

};