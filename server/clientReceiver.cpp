#include "clientReceiver.h"
#include "admin.h"

ClientReceiver::ClientReceiver(Protocol& protocol, const std::string& clientName, Admin& admin, std::shared_ptr<Queue<Message>> requests, std::function<void(const std::string&)> onNameReceived)
    : protocol(protocol), clientName(clientName), active(true), admin(admin), requests(std::move(requests)), onNameReceived(std::move(onNameReceived)) {}

ClientReceiver::~ClientReceiver() {}

void ClientReceiver::run() {
    try {
        handleClientName();
    } catch (...) {
        return;  // Ya se logueó y envió la respuesta de error
    }

    receiveMessagesLoop();
}

void ClientReceiver::handleClientName() {
    try {
        if (!onNameReceived) return;

        clientName = protocol.recv_name();
        onNameReceived(clientName);

        protocol.send_response({
            Type::NAME,
            0,
            {},
            "Client successfully connected"
        });
    } catch (const std::exception& e) {
        handleNameError(e.what());
        throw; // para que `run()` decida si continúa o no
    } catch (...) {
        handleNameError("Unknown error");
        throw;
    }
}

void ClientReceiver::handleNameError(const std::string& errorMsg) {
    active = false;
    protocol.send_response({
        Type::NAME,
        1,
        {},
        "Error with the name: " + errorMsg
    });
    std::cerr << "Exception receiving name in ClientReceiver: " << errorMsg << std::endl;
}

void ClientReceiver::receiveMessagesLoop() {
    while (active) {
        try {
            Message message = protocol.recv_message();
            message.clientName = clientName;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                requests->push(message);
            }
        } catch (const std::exception& e) {
            handleDisconnection(e.what());
        } catch (...) {
            handleDisconnection("Unknown exception");
        }
    }
}

void ClientReceiver::handleDisconnection(const std::string& reason) {
    requests->push({
        Type::DISCONNECT,0,"",{}, clientName
    });
    std::cerr << "[" << clientName << "] Exception receiving message in ClientReceiver: " << reason << std::endl;
}

void ClientReceiver::changeQueue(std::shared_ptr<Queue<Message>> newQueue) {
    std::lock_guard<std::mutex> lock(queueMutex);
    requests = newQueue;
}

void ClientReceiver::stop() {
    active = false;
    protocol.close();
}