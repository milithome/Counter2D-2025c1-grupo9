#include "clientHandler.h"

ClientHandler::ClientHandler(
    Protocol protocol,
    Admin& admin,
    std::function<void(std::string, std::shared_ptr<ClientHandler>)> onRegister)
    : protocol(std::move(protocol)),
        clientName(""),
        active(true),
        admin(admin),
        onRegister(std::move(onRegister)) {}

ClientHandler::ClientHandler(
    Protocol protocol,
    const std::string& clientName,
    Admin& admin)
    : protocol(std::move(protocol)),
        clientName(clientName),
        active(true),
        admin(admin),
        onRegister(nullptr) {}

ClientHandler::~ClientHandler() {
    std::cout << "ClientHandler destructor called." << std::endl;
}

void ClientHandler::run() {
    try {
        if (onRegister) {
            clientName = protocol.recv_name();
            onRegister(clientName, shared_from_this());
        }
        while (active) {
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
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in ClientHandler: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in ClientHandler." << std::endl;
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
        std::cout << "Lobby creation failed" << std::endl;
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
    std::cout << "Lobby created successfully" << std::endl;
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
        std::vector<std::string> lobbies = admin.listLobbies();
        Response response = {
            Type::LIST,
            static_cast<uint16_t>(lobbies.size()),
            {},
            lobbies,
            0,  // 0 para éxito
            ""
        };
        protocol.send_response(response);
    } catch (const std::exception& e) {
        std::cerr << "Error listing lobbies: " << e.what() << std::endl;
        Response response = {
            Type::LIST,
            0,  // Tamaño 0 en caso de error
            {},
            {},
            1,  // 1 para error
            "Error listing lobbies"
        };
        protocol.send_response(response);
    }
}

void ClientHandler::stop() {
    active = false;
}