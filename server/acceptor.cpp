#include "acceptor.h"


Acceptor::Acceptor(const std::string& port) : skt(port.c_str()), active(true) {}

void Acceptor::run() {
    while (active) {
        try {
            Socket client = skt.accept();
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
