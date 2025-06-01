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

ClientHandler::~ClientHandler() {}

void ClientHandler::run() {
    try {
        clientName = protocol.recv_name();
        onRegister(clientName, shared_from_this());

        while (active) {
            Message message = protocol.recv_message();
            handle_message(message);
        }

        admin.removeHandler(clientName);
    } catch (const std::exception& e) {
        std::cerr << "Exception in " << clientName << " ClientHandler: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in ClientHandler." << std::endl;
    }
}

void ClientHandler::handle_message(const Message& message) {
    switch (message.type) {
        case Type::CREATE:
            handle_create(message.name);
            break;
        case Type::JOIN:
            handle_join(message.name);
            break;
        case Type::LIST:
            handle_list();
            break;
        case Type::DISCONNECT:
            send_simple_response(Type::DISCONNECT, "Disconnect successful",0);
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
        LobbyChannels queues = admin.joinLobby(name, clientName, protocol);
        send_simple_response(Type::CREATE, "Lobby created successfully", 0);
        enter_lobby(name, *queues.toLobby, *queues.fromLobby);
    } catch (const std::exception& e) {
        send_simple_response(Type::CREATE, "Error creating lobby: " + std::string(e.what()), 1);
    }
}

void ClientHandler::handle_join(const std::string& name) {
    try {
        LobbyChannels queues = admin.joinLobby(name, clientName, protocol);
        send_simple_response(Type::JOIN, "Joined lobby", 0);
        enter_lobby(name, *queues.toLobby, *queues.fromLobby);
    } catch (const std::exception& e) {
        std::cerr << "Error in lobby: " << e.what() << std::endl;
        send_simple_response(Type::JOIN, "Error joining lobby: " + std::string(e.what()), 1);
    }
}

void ClientHandler::enter_lobby(const std::string& lobbyName, Queue<LobbyRequest>& toLobby, Queue<LobbyRequest>& fromLobby) {
    bool inLobby = true;
    while (inLobby) {
        if (protocol.has_data()) {
            Message msg = protocol.recv_message();
            if (handle_lobby_client_message(msg, toLobby, inLobby)) {
                continue;
            }
        }

        LobbyRequest event;
        if (fromLobby.try_pop(event)) {
            switch (event.type)
            {
            case LobbyRequestType::START_LOBBY:
                inLobby = false;
                send_simple_response(Type::START, "Game started",0);
                handle_game(lobbyName);
                break;
            case LobbyRequestType::READY_LOBBY:
                send_simple_response(Type::LOBBY_READY, "Lobby is ready", 0);
                break;
            default:
                break;
            }
        }
    }
}

bool ClientHandler::handle_lobby_client_message(const Message& msg, Queue<LobbyRequest>& toLobby, bool& inLobby) {
    Response response;
    switch (msg.type) {
        case Type::LEAVE:
            toLobby.push({LobbyRequestType::LEAVE_LOBBY, clientName});
            inLobby = false;
            send_simple_response(Type::LEAVE, "Leave successful",0);
            return true;
        case Type::START:
            toLobby.push({LobbyRequestType::START_LOBBY, clientName});
            return false;
        case Type::DISCONNECT:
            active = false;
            inLobby = false;
            send_simple_response(Type::DISCONNECT, "Disconnect successful",0);
            return true;
        default:
            break;
    }
    return false;
}

void ClientHandler::handle_game(const std::string& name) {
    try {
        GameChannels queues = admin.joinGame(name, clientName, protocol);
        Queue<ActionRequest>& toGame = *queues.toGame;
        Queue<Response>& fromGame = *queues.fromGame;
        
        bool inGame = true;
        while (inGame) {
            if (protocol.has_data()) {
                Message msg = protocol.recv_message();
                if (handle_game_client_message(msg, toGame, inGame)) {
                    continue;
                }
            }

            Response response;
            if (fromGame.try_pop(response)) {
                switch (response.type) {
                    case Type::INITIAL_DATA:
                        protocol.send_response(response);
                        break;
                    case Type::STATE:
                        protocol.send_response(response);
                        break;
                    case Type::FINISH:
                        send_simple_response(Type::FINISH, "Game finished",0);
                        inGame = false;
                        break;
                    default:
                        std::cerr << "Unknown response type in game: " << response.type << std::endl;
                        break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } catch (const std::exception& e) {
        std::cerr << "Exception in " << clientName << " Game: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Game." << std::endl;
    }
}

bool ClientHandler::handle_game_client_message(const Message& msg, Queue<ActionRequest>& toGame, bool& inGame) {
    ActionRequest event;
    switch (msg.type) {
        case Type::LEAVE:
            inGame = false;
            return true;
        case Type::ACTION:
            event = {msg.action, clientName};
            toGame.push(event);
            return false;
        case Type::DISCONNECT:
            active = false;
            inGame = false;
            send_simple_response(Type::DISCONNECT, "Disconnect successful",0);
            return true;
        default:
            break;
    }
    return false;
}

void ClientHandler::handle_list() {
    try {
        std::vector<std::string> lobbies = admin.listLobbies();

        if (lobbies.empty()) {
            send_simple_response(Type::LIST, "No lobbies available", 0);
            return;
        }
        
        Response response = {
            Type::LIST,
            0,
            LobbyList{lobbies},
            "Lobbies listed successfully"
        };
        protocol.send_response(response);

    } catch (const std::exception& e) {
        Response response = {
            Type::LIST,
            1,
            {},
            "Error listing lobbies: " + std::string(e.what())
        };
        protocol.send_response(response);
    }
}

void ClientHandler::send_simple_response(Type type, const std::string& msg, uint8_t result) {
    Response response = {
        type,
        result,
        {},
        msg
    };
    
    protocol.send_response(response);
}

void ClientHandler::stop() {
    active = false;
}
