#include "acceptor.h"


Acceptor::Acceptor(const std::string& port, Admin& admin) : skt(port.c_str()), active(true), admin(admin) {}

void Acceptor::run() {
    while (active) {
        try {
            Socket client = skt.accept();
            Protocol protocol(std::move(client));
            std::string clientName = "Client"; // aca falta establecer en el protocolo que se pase el nombre
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
