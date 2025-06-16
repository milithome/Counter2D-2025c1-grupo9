#include "clientReceiver.h"
#include "admin.h"

ClientReceiver::ClientReceiver(Protocol& protocol, const std::string& clientName, Admin& admin, std::shared_ptr<Queue<Message>> requests, std::function<void(const std::string&)> onNameReceived)
    : protocol(protocol), clientName(clientName), active(true), admin(admin), requests(std::move(requests)), onNameReceived(std::move(onNameReceived)) {}

ClientReceiver::~ClientReceiver() {}

void ClientReceiver::run() {
    try {
        if (onNameReceived) {
            clientName = protocol.recv_name();
            onNameReceived(clientName);
        }

        protocol.send_response({
            Type::NAME,
            0,
            {},
            "Client successfully connected"
        });

    } catch (const std::exception& e) {
        std::cerr << "Exception receiving name in ClientReceiver: " << e.what() << std::endl;
        active = false;
        protocol.send_response({
            Type::NAME,
            1,
            {},
            "Error with the name: " + std::string(e.what())
        });
    } catch (...) {
        active = false;
        protocol.send_response({
            Type::NAME,
            1,
            {},
            "Error with the name:"
        });
        std::cerr << "Unknown exception in ClientReceiver." << std::endl;
    }
    
    while (active) {
        try {
            Message message = protocol.recv_message();
            message.clientName = clientName;
            
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                requests->push(message);
            }

            if (message.type == Type::DISCONNECT) {
                active = false;
            }
        } catch (const std::exception& e) {
            active = false;
            admin.removeClient(clientName,true);
            std::cerr << "[" << clientName << "]" << "Exception receiving message in ClientReceiver: " << e.what() << std::endl;
        } catch (...) {
            active = false;
            admin.removeClient(clientName,true);
            std::cerr << "Unknown exception in ClientReceiver." << std::endl;
        }
    }
}

void ClientReceiver::changeQueue(std::shared_ptr<Queue<Message>> newQueue) {
    std::lock_guard<std::mutex> lock(queueMutex);
    requests = newQueue;
}

void ClientReceiver::stop() {
    active = false;
}