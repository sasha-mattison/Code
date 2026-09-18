#include "application.h"

Application::Application() 
    : reqM(endpoint) {}

std::string Application::chooseModel() {
    std::string choice;
    std::cout << "Enter model name: ";
    std::getline(std::cin, choice);

    return choice;
}

void Application::run() {

    reqM.setModel(chooseModel());

    while (true) {
        reqM.message();
    }
}