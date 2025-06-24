#include "clientReceiver.h"

ClientReceiver::ClientReceiver(Protocol& protocol, const std::string& clientName, std::shared_ptr<Queue<Message>> requests, std::function<void(const std::string&)> onNameReceived)
    : protocol(protocol), clientName(clientName), active(true), requests(std::move(requests)), onNameReceived(std::move(onNameReceived)) {}

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
    try {
        while (active) {
            Message message = protocol.recv_message();
            message.clientName = clientName;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                requests->push(message);
            }
        }
    } catch (const ClientClosedConnection&) {
        //std::cout << "[" << clientName << "] Receiver closed because the client closed the connection" << std::endl;
        handleDisconnection();
    } catch (const ServerClosedConnection&) {
        //std::cout << "[" << clientName << "] Receiver closed because the server closed the connection." << std::endl;
        return;
    } catch (...) {
        //std::cerr << "[" << clientName << "] Unknown error in receiveMessagesLoop()." << std::endl;
        handleDisconnection();
    }
}

void ClientReceiver::handleDisconnection() {
    requests->push({
        Type::DISCONNECT,0,"",{}, clientName
    });
}

void ClientReceiver::changeQueue(std::shared_ptr<Queue<Message>> newQueue) {
    std::lock_guard<std::mutex> lock(queueMutex);
    requests = newQueue;
}

void ClientReceiver::stop() {
    active = false;
    protocol.close();
}