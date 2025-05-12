#include "acceptor.h"


Acceptor::Acceptor(const std::string& port, Admin& admin) : skt(port.c_str()), active(true), admin(admin) {}

void Acceptor::run() {
    while (active) {
        try {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));
            std::string clientName = "Client";
            admin.createHandler(clientName, std::move(protocol));
            admin.startHandler(clientName);
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
