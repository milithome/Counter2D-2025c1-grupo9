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
            handle_message(message);
        }

        admin.removeHandler(clientName);
    } catch (const std::exception& e) {
        std::cerr << "Exception in ClientHandler: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in ClientHandler." << std::endl;
    }
}

void ClientHandler::handle_message(const Message& message) {
    switch (message.type) {
        case Type::CREATE:
            handle_create(message.name);
            handle_join(message.name);
            break;
        case Type::JOIN:
            handle_join(message.name);
            break;
        case Type::LIST:
            handle_list();
            break;
        case Type::DISCONNECT:
            send_simple_response(Type::DISCONNECT, "Disconnect successful");
            active = false;
            break;
        default:
            std::cerr << "Unknown message type received." << std::endl;
            break;
    }
}

void ClientHandler::handle_create(const std::string& name) {
    try {
        admin.createLobby(name);
    } catch (const std::exception& e) {
        std::cerr << "Error creating lobby: " << e.what() << std::endl;
    }
}

void ClientHandler::handle_join(const std::string& name) {
    try {
        LobbyChannels queues = admin.joinLobby(name, clientName, protocol);
        send_simple_response(Type::JOIN, "Joined lobby");

        Queue<LobbyEvent>& toLobby = *queues.toLobby;
        Queue<LobbyEvent>& fromLobby = *queues.fromLobby;

        bool inLobby = true;
        while (inLobby) {
            if (protocol.has_data()) {
                Message msg = protocol.recv_message();
                if (handle_lobby_client_message(msg, toLobby, inLobby)) {
                    continue;
                }
            }

            LobbyEvent event;
            if (fromLobby.try_pop(event) && event.type == LobbyEventType::START) {
                inLobby = false;
                handle_game(name);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in lobby: " << e.what() << std::endl;
    }
}

bool ClientHandler::handle_lobby_client_message(const Message& msg, Queue<LobbyEvent>& toLobby, bool& inLobby) {
    Response response;
    switch (msg.type) {
        case Type::LEAVE:
            toLobby.push({LobbyEventType::LEAVE, clientName});
            inLobby = false;
            send_simple_response(Type::LEAVE, "Leave successful");
            return true;
        case Type::DISCONNECT:
            active = false;
            inLobby = false;
            send_simple_response(Type::DISCONNECT, "Disconnect successful");
            return true;
        default:
            break;
    }
    return false;
}

void ClientHandler::handle_game(const std::string& name) {
    try {
        send_simple_response(Type::START, "Game started");

        GameChannels queues = admin.joinGame(name, clientName, protocol);
        Queue<ActionEvent>& toGame = *queues.toGame;
        Queue<ActionEvent>& fromGame = *queues.fromGame;

        bool inGame = true;
        while (inGame) {
            if (protocol.has_data()) {
                Message msg = protocol.recv_message();
                if (handle_game_client_message(msg, toGame, inGame)) {
                    continue;
                }
            }

            ActionEvent event;
            if (fromGame.try_pop(event) && event.action.type == ActionType::FINISH) {
                send_simple_response(Type::FINISH, "Game finished");
                inGame = false;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in Game: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Game." << std::endl;
    }
}

bool ClientHandler::handle_game_client_message(const Message& msg, Queue<ActionEvent>& toGame, bool& inGame) {
    Response response;
    switch (msg.type) {
        case Type::LEAVE:
            inGame = false;
            return true;
        case Type::ACTION: {
            ActionEvent event{msg.action, clientName};
            toGame.push(std::move(event));
            return false;
        }
        case Type::DISCONNECT:
            active = false;
            inGame = false;
            send_simple_response(Type::DISCONNECT, "Disconnect successful");
            return true;
        default:
            break;
    }
    return false;
}

void ClientHandler::handle_list() {
    try {
        std::vector<std::string> lobbies = admin.listLobbies();
        Response response = {
            Type::LIST,
            static_cast<uint16_t>(lobbies.size()),
            {},
            lobbies,
            0,
            ""
        };
        protocol.send_response(response);
    } catch (const std::exception& e) {
        std::cerr << "Error listing lobbies: " << e.what() << std::endl;
        Response response = {
            Type::LIST,
            0,
            {},
            {},
            1,
            "Error listing lobbies"
        };
        protocol.send_response(response);
    }
}

void ClientHandler::send_simple_response(Type type, const std::string& msg) {
    Response response = {
        type,
        0,
        {},
        {},
        0,
        msg
    };
    protocol.send_response(response);
}

void ClientHandler::stop() {
    active = false;
}
