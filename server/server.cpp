#include "server.h"

Server::Server() : acceptor(DEFAULT_PORT, admin) {} 

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
            std::cout << "Server shutting down..." << std::endl;
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
    acceptor.join();
    admin.stop();
}