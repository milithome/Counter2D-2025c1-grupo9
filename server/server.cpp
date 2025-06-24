#include "server.h"

Server::Server(const ServerConfig& config) : admin(config), acceptor(config.port, admin) {} 

Server::~Server() {}

void Server::start(std::istream& input) {
    acceptor.start();

    bool active = true;
    while (active) {
        std::string command;
        std::getline(input, command);

        if (command == "q") {
            active = false;
            acceptor.stop();
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
    
    admin.stop();
    acceptor.join();
}