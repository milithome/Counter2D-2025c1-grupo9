#include "acceptor.h"
#include <thread>

Acceptor::Acceptor(const std::string& port, Admin& admin) : skt(port.c_str()), active(true), admin(admin) {}

void Acceptor::run() {
    try {
        while (active) {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));
            auto handler = std::make_shared<ClientHandler>(std::move(protocol), *this);
            admin.startHandler(handler);
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
