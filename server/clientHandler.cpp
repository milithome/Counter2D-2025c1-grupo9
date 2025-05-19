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
                    handle_join(message.name);
                    break;
                case Type::JOIN:
                    handle_join(message.name);
                    break;
                case Type::LIST:
                    handle_list();
                    break;
                default:
                    std::cerr << "Unknown message type received." << std::endl;
            }
        }

        admin.removeHandler(clientName);
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
        std::cerr << "Error in create lobby" << std::endl;
    }
}

void ClientHandler::handle_join(const std::string& name) {
    try {
        LobbyChannels queues = admin.joinLobby(name, clientName, protocol);
        Response response = {
            Type::JOIN,
            0,
            {},
            {},
            0, // exito
            ""
        };
        protocol.send_response(response);

        Queue<LobbyEvent>& toLobby  = *queues.toLobby;
        Queue<LobbyEvent>& fromLobby = *queues.fromLobby;


        bool inLobby = true;
        while (inLobby) {
            Response response;
            if (protocol.has_data()) {
                Message msg = protocol.recv_message();
                if (msg.type == Type::LEAVE) {
                    toLobby.push({LobbyEventType::LEAVE, clientName});
                    inLobby = false;
                    response = {
                            Type::LEAVE,
                            0, {}, {}, 0, "Leave successfull"
                        };
                    protocol.send_response(response);
                    continue;
                }
            }

            LobbyEvent event;
            if (fromLobby.try_pop(event)) {
                switch (event.type) {
                    case LobbyEventType::START:                        
                        inLobby = false;
                        handle_game(name);
                        break;
                    default:
                        break;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in lobby: " << e.what() << std::endl;
    }
}

void ClientHandler::handle_game(const std::string& name){
    try {
        Response response = {
            Type::START,
            0,
            {},
            {},
            0, // exito
            ""
        };
        protocol.send_response(response);
        GameChannels queues = admin.joinGame(name, clientName, protocol);
        Queue<ActionEvent>& toGame  = *queues.toGame;
        Queue<ActionEvent>& fromGame = *queues.fromGame;


        bool inGame = true;
        while (inGame) {
            if (protocol.has_data()) {
                Message msg = protocol.recv_message();

                if (msg.type == Type::LEAVE) {
                    inGame = false;
                    break;
                }

                if (msg.type == Type::ACTION) {
                    ActionEvent event;
                    event.action = msg.action;
                    event.playerName = clientName;
                    toGame.push(std::move(event));
                }
            }

            ActionEvent outgoingEvent;
            if (fromGame.try_pop(outgoingEvent)) {
                switch (outgoingEvent.action.type) {
                    case ActionType::FINISH:
                        response = {
                            Type::FINISH,
                            0, {}, {}, 0, "Game finish"
                        };
                        protocol.send_response(response);
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        inGame = false;
                        break;
                    default:
                        break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in Game: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Game: " << std::endl;
    }
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