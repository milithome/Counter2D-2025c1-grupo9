#include "acceptor.h"
#include <thread>

Acceptor::Acceptor(const std::string& port, Admin& admin) 
    : skt(port.c_str()), active(true), admin(admin) {}

void Acceptor::run() {
    try {
        while (active) {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));

            auto handler = std::make_shared<ClientHandler>(
                std::move(protocol),
                admin,
                [this, weak_handler = std::weak_ptr<ClientHandler>{}](std::string name, std::shared_ptr<ClientHandler> handler) mutable {
                    weak_handler = handler;
                    admin.registerHandler(name, handler);
                    unnamedHandlers.erase(handler);
                }
            );
            unnamedHandlers.insert(handler);
            handler->start();
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
    for (const auto& handler : unnamedHandlers) {
            handler->stop();
    }
    skt.shutdown(SHUT_RDWR);
    skt.close();  
}

Acceptor::~Acceptor() {}
