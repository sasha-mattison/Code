#include "requestManager.h"

RequestManager::RequestManager(std::string modelName, std::string endpointURL)
    : model(modelName), endpoint(endpointURL), client(endpointURL) {}

RequestManager::RequestManager(std::string endpointURL)
    :endpoint(endpointURL), model(""), client(endpointURL) {}

void RequestManager::setModel(std::string modelName) {
    model = modelName;
}

void RequestManager::chooseModel() {
    std::cout << "Choose a model: \n";
    int modelNum = 1;
    auto modelList = getModelList();
    for (std::string m : modelList) {
        std::cout << std::to_string(modelNum) << ": " << m << std::endl;
        modelNum++;
    }
    std::cout << "Choice: ";
    std::string userIn;
    std::cin >> userIn;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    try {
        int userChoice = std::stoi(userIn);
        userChoice < modelList.size() ? setModel(modelList[userChoice-1]) : chooseModel();

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument provided\n";
        chooseModel();
    }

}

std::string RequestManager::getUserPrompt() {
    std::string prompt;
    std::cout << "Enter Prompt: ";
    std::getline(std::cin, prompt);

    return prompt;
}

void RequestManager::message() {

    std::string prompt = getUserPrompt();

    Commands command = getCommand(prompt);

    switch(command) {
        case Commands::EXIT: {
            std::cout << "Exiting...\n";
            std::exit(EXIT_SUCCESS);
            break;
        }
        case Commands::MODEL: {
            chooseModel();
            return;
            break;
        }
        case Commands::EFFORT: {
            std::string mod = 
        }
        default: {
            break;
        }
    }

    Request req(model, prompt, Effort::LOW);
    json out = req.build();

    httplib::Request httpRequest;

    httpRequest.method = "POST";
    httpRequest.path = "/api/generate";
    httpRequest.headers.emplace("Content-Type", "application/json");
    httpRequest.body = out.dump();

    std::string buf;
    std::string fullText;
    json lastChunk;

    httpRequest.content_receiver = [&](const char *data, size_t dataLength, uint64_t, uint64_t) {
        buf.append(data, dataLength);

        size_t pos;
        while ((pos = buf.find('\n')) != std::string::npos) {
            std::string line = buf.substr(0, pos);
            buf.erase(0, pos + 1);
            if (line.empty()) continue;

            try {
                json chunk = json::parse(line);

                if (chunk.contains("response")) {
                    std::string piece = chunk["response"].get<std::string>();
                    fullText += piece;
                    std::cout << piece << std::flush;
                }

                lastChunk = chunk;

                if (chunk.value("done", false)) {
                    std::cout << std::endl;
                }
            } catch (const json::parse_error &e) {
                std::cerr << "Failed to parse chunk: " << e.what() << std::endl;
            }
        }
        return true;
    };

    httplib::Response res;
    httplib::Error err;

    if (!client.send(httpRequest, res, err)) {
        std::cerr << "Request failed: " << httplib::to_string(err) << std::endl;
        return;
    }
    if (res.status != 200) {
        std::cerr << "Unexpected status: " << res.status << std::endl;
        std::cerr << res.body << std::endl;
        return;
    }

    int tokenCount = lastChunk.value("eval_count", 0);
    long double responseTime = (lastChunk.value("prompt_eval_duration", 0LL) + lastChunk.value("eval_duration", 0LL)) / 1e9L;
    double tokensPerSecond = tokenCount/responseTime;

    totalTokenCount += tokenCount;


    std::cout << "\nOutput tokens: " << tokenCount << std::endl;
    std::cout << "\nTotal output tokens: " << totalTokenCount << std::endl;
    std::cout << "\nResponse time: " << responseTime << std::endl;
    std::cout << "\nTokens per second: " <<tokensPerSecond << std::endl;


}

std::vector<std::string> RequestManager::getModelList() {
    std::string urlDir = "/api/tags";

    auto response = client.Get(urlDir);

    if (!response) {
        throw std::runtime_error("HTTP request failed: " +
            httplib::to_string(response.error()));
    }
    if (response->status != 200) {
        throw std::runtime_error("Unexpected status: " + std::to_string(response->status));
    }

    json rawModelList = json::parse(response->body);

    std::vector<std::string> list;
    for (const auto& model : rawModelList["models"]) {
        list.push_back(model["name"].get<std::string>());
    }

    return list;
}


Commands RequestManager::getCommand(std::string message) {
    constexpr char commandPrefix = '/';

    int index = message.find(commandPrefix);
    if (index == std::string::npos) return Commands::None;

    int start = index + 1;
    int end = message.find(' ', start);
    std::string commandString = message.substr(start, end - start);

    static const std::unordered_map<std::string, Commands> commandMap = {
        {"effort", Commands::EFFORT},
        {"model", Commands::MODEL},
        {"exit", Commands::EXIT}
    };

    auto it = commandMap.find(commandString);
    return (it != commandMap.end()) ? it->second : Commands::None;
}