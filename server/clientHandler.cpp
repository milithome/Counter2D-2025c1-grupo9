#include "clientHandler.h"

ClientHandler::ClientHandler(Protocol protocol, std::string clientName, Admin& admin) : 
    protocol(std::move(protocol)), clientName(clientName), active(true), admin(admin) {}

void ClientHandler::run() {
    while (active) {
        try {
            Message message = protocol.recv_message();
            switch (message.type) {
                case Type::CREATE:
                    handle_create(message.name);
                    break;
                case Type::JOIN:
                    active = !handle_join(message.name);
                    break;
                case Type::LIST:
                    handle_list();
                    break;
                default:
                    std::cerr << "Unknown message type received." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error receiving message: " << e.what() << std::endl;
            active = false;
        }
    }
     
}

void ClientHandler::handle_create(const std::string& name) {
    try {
        admin.createLobby(name);
    } catch (const std::exception& e) {
        std::cerr << "Error creating lobby: " << e.what() << std::endl;
        Response response = {
            Type::CREATE,
            0,
            {},
            {},
            1, // error
            "Error creating lobby"
        };
        protocol.send_response(response);
        return;
    }
    Response response = {
        Type::CREATE,
        0,
        {},
        {},
        0, // exito
        ""
    };
    protocol.send_response(response);
}

bool ClientHandler::handle_join(const std::string& name) {
    try {
        admin.joinLobby(name, clientName, std::move(protocol));
    } catch (const std::exception& e) {
        std::cerr << "Error joining lobby: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

void ClientHandler::handle_list() {
    try {
        std::vector<std::string> lobbies;
        admin.listLobbies(lobbies);
    } catch (const std::exception& e) {
        std::cerr << "Error listing lobbies: " << e.what() << std::endl;
        Response response = {
            Type::LIST,
            0,
            {},
            {},
            1, // error
            "Error listing lobbies"
        };
        protocol.send_response(response);
        return;
    }
    
    Response response = {
        Type::LIST,
        0,
        {},
        {"partida1", "partida2"},
        0, // exito
        ""
    };
    protocol.send_response(response);
}
