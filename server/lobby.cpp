#include "lobby.h"

Lobby::Lobby(const std::string& name, Admin& admin) 
    : name(name), players(), admin(admin), maxPlayers(2), toLobby(std::make_shared<Queue<LobbyEvent>>(100)), fromPlayers(), active(true) {}

void Lobby::run() {
        try {
            while (active) {
                LobbyEvent event;
                if (toLobby->try_pop(event)) {
                    switch (event.type) {
                        case LobbyEventType::JOIN:
                            handle_join_event(event.playerName);
                            break;
                        case LobbyEventType::LEAVE:
                            handle_leave_event(event.playerName);
                            break;
                        default:
                            break;
                    }
    
                    broadcast_lobby_state();
    
                    if (players.size() == maxPlayers) {
                        active = false;

                        admin.startGame(name);

                        for (auto& [name, queue] : fromPlayers) {
                            LobbyEvent event = {
                                LobbyEventType::START,
                                name
                            };
                            queue->push(event);
                        }
                    }
                
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "Exception in Lobby::run: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception in Lobby::run" << std::endl;
        }
    }
    
void Lobby::handle_join_event(const std::string& playerName) {
    /*
    auto it = players.find(playerName);
    if (it != players.end()) {}
    */
    std::cout << "Player " << playerName << " joined the lobby." << std::endl;
}

void Lobby::handle_leave_event(const std::string& playerName) {
    auto it = players.find(playerName);
    if (it != players.end()) {
        players.erase(it);
    } else {
        std::cerr << "Player " << playerName << " not found in the lobby." << std::endl;
    }
    std::cout << "Player " << playerName << " left the lobby." << std::endl;
}

void Lobby::broadcast_lobby_state() {
    std::vector<std::string> playerNames;
    for (const auto& pair : players) {
        playerNames.push_back(pair.first);
    }

    for (auto& pair : players) {
        Response response = {
            Type::STATE_LOBBY,
            0,
            static_cast<uint16_t>(playerNames.size()),
            {},
            {},
            playerNames,
            ""
        };
        pair.second.send_response(response);
    }
}
    
LobbyChannels Lobby::add_player(Protocol& player, const std::string& playerName) {
    if (maxPlayers <= players.size()) {
        return {};
    }
    
    players.emplace(playerName, player);

    auto fromPlayerQueue = std::make_shared<Queue<LobbyEvent>>(100);
    fromPlayers.emplace(playerName,fromPlayerQueue);

    toLobby->push({LobbyEventType::JOIN, playerName});

    return LobbyChannels{
        toLobby,
        fromPlayerQueue
    };
}

void Lobby::stop() {
    active = false;
    toLobby->close();
    for (auto& [_, queue] : fromPlayers) {
        queue->close();
    }
}

Lobby::~Lobby() {
    std::cout << "Lobby destructor called." << std::endl;
}