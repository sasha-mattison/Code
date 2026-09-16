#pragma once

#include <json.hpp>
#include <string>

using json = nlohmann::json;

enum class Effort {
    NONE,
    LOW,
    MEDIUM,
    HIGH,
    MAX
};

struct Request {

    std::string model;
    std::string prompt;
    Effort effort;

    Request(std::string userPrompt, Effort modelEffort); 
    json build();
};

struct Response {

};