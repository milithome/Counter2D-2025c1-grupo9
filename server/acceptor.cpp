#include "acceptor.h"
#include <thread>

Acceptor::Acceptor(const std::string& port, Admin& admin,std::vector<std::shared_ptr<ClientHandler>>& handlers) 
    : skt(port.c_str()), active(true), admin(admin), handlers(handlers) {}

void Acceptor::run() {
    try {
        while (active) {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));

            auto handler = std::make_shared<ClientHandler>(
                std::move(protocol),
                admin,
                [this](std::string name, std::shared_ptr<ClientHandler> handler) {
                    admin.registerHandler(name, handler);
                });
            handlers.push_back(handler);
            handler->start();
            std::cout << "Accepted connection from client." << std::endl;
        }
    } catch (const std::exception& e) {
        if (active) {
            std::cerr << "Error accepting connection: " << e.what() << std::endl;
        }
    } catch (...) {
        if (active) {
            std::cerr << "Unknown error accepting connection." << std::endl;
        }
    }
}

void Acceptor::stop() {
    active = false;
    skt.shutdown(SHUT_RDWR);
    skt.close();  
}

Acceptor::~Acceptor() {
    std::cout << "Acceptor destructor called." << std::endl;
}
