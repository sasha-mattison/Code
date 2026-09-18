#include "requestManager.h"

RequestManager::RequestManager(std::string modelName, std::string endpointURL)
    : model(modelName), endpoint(endpointURL), client(endpointURL) {}

RequestManager::RequestManager(std::string endpointURL)
    :endpoint(endpointURL), model(""), client(endpointURL) {}

void RequestManager::setModel(std::string modelName) {
    model = modelName;
}

std::string RequestManager::getUserPrompt() {
    std::string prompt;
    std::cout << "Enter Prompt: ";
    std::getline(std::cin, prompt);

    return prompt;
}

void RequestManager::message() {

    std::string prompt = getUserPrompt();

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