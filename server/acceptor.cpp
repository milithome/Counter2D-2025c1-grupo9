#include "acceptor.h"
#include <thread>

Acceptor::Acceptor(const std::string& port, Admin& admin) 
    : skt(port.c_str()), active(true), admin(admin) {}

void Acceptor::run() {
    try {
        while (active) {
            Socket clientSkt = skt.accept();
            Protocol protocol(std::move(clientSkt));
            auto client = admin.createClient(std::move(protocol));
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

Acceptor::~Acceptor() {}
