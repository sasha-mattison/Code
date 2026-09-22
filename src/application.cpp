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

    // for (std::string m : reqM.getModelList()) {
    //     std::cout << m << std::endl;
    // }

    //reqM.setModel(chooseModel());

    reqM.chooseModel();
    //reqM.setModel("gemma3:270m");

    while (true) {
        reqM.message();
    }
}