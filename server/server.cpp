#include "server.h"

Server::Server() : acceptor(DEFAULT_PORT) {} 

Server::~Server() {}

void Server::start() {
    acceptor.start();

    bool active = true;
    while (active) {
        std::string command;
        std::cin >> command;

        if (command == "q") {
            active = false;
            acceptor.stop();
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
    acceptor.join();
}