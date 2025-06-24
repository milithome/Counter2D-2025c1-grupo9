#include "clientSender.h"

ClientSender::ClientSender(Protocol& protocol, const std::string& clientName, std::shared_ptr<Queue<Response>> requests)
    : protocol(protocol), clientName(clientName), active(true), requests(std::move(requests)) {}

ClientSender::~ClientSender() {}

void ClientSender::run() {
    try {
        while (active) {
            Response response = requests->pop();
            protocol.send_response(response);
        }
    } catch (const ClosedQueue&) {
        std::cout << "[" << clientName << "] Sender closed." << std::endl;
    } catch (...) {
        std::cerr << "[" << clientName << "] Unknown exception in ClientSender." << std::endl;
    }
}

void ClientSender::stop() {
    active = false;
    requests->close();
}