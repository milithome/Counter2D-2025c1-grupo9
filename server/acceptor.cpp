#include "acceptor.h"


Acceptor::Acceptor(const std::string& port) : skt(port.c_str()), active(true) {}

void Acceptor::run() {
    while (active) {
        try {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));
            std::string clientName = "Client";
            ClientHandler* clientHandler = new ClientHandler(std::move(protocol), clientName);
            clientHandler->start();
            std::cout << "Accepted connection from client." << std::endl;
        } catch (const std::exception& e) {
            if (active) {
                std::cerr << "Error accepting connection: " << e.what() << std::endl;
            }
        }
    }
}

void Acceptor::stop() {
    active = false;
    skt.shutdown(SHUT_RDWR);
    skt.close();
   
}
