#include "request.h"

Request::Request(std::string modelName, std::string userPrompt, Effort modelEffort)
    : model(modelName), prompt(userPrompt), effort(modelEffort) {}

json Request::build() {

    json request;

    bool thinkingEnabled = false; //TODO Change
    std::string effortString;
    
    switch (effort) {
        case Effort::NONE:
            thinkingEnabled = false;
            effortString = "low";
            break;
        case Effort::LOW:
            effortString = "low";
            break;
        case Effort::MEDIUM:
            effortString = "medium";
            break;
        case Effort::HIGH:
            effortString = "high";
            break;
        case Effort::MAX:
            effortString = "max";
            break;
    }

    

    request.emplace("model", model);

    if (thinkingEnabled) 
        request.emplace("think", effortString);
    else
        request.emplace("think", thinkingEnabled);
    
    request.emplace("prompt", prompt);
    request.emplace("stream", true);


    return request;
}

