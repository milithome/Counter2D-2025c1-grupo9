#include "lobby.h"

Lobby::Lobby(const std::string& name, Admin& admin) 
    : name(name), players(), admin(admin), maxPlayers(3), toLobby(std::make_shared<Queue<LobbyRequest>>(100)), fromPlayers(), active(true) {}

void Lobby::run() {
        try {
            while (active) {
                LobbyRequest event;
                if (toLobby->try_pop(event)) {
                    switch (event.type) {
                        case LobbyRequestType::JOIN_LOBBY:
                            handle_join_event();
                            break;
                        case LobbyRequestType::LEAVE_LOBBY:
                            handle_leave_event(event.playerName);
                            break;
                        case LobbyRequestType::START_LOBBY:
                            if (start_game()) {
                                continue;
                            }
                            break;
                        default:
                            break;
                    }
                    broadcast_lobby_state();
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
    
void Lobby::handle_join_event() {
    if (players.size() == maxPlayers) {
        for (auto& [name, queue] : fromPlayers) {
            LobbyRequest event = {
                LobbyRequestType::READY_LOBBY,
                name
            };
            queue->push(event);
        }
        return;
    }
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
            StateLobby{playerNames},
            "Lobby state updated for player " + pair.first + ". Current players: " + std::to_string(playerNames.size())
        };

        pair.second.send_response(response);
    }
}
    
LobbyChannels Lobby::add_player(Protocol& player, const std::string& playerName) {
    if (maxPlayers <= players.size()) {
        return {};
    }
    
    players.emplace(playerName, player);

    auto fromPlayerQueue = std::make_shared<Queue<LobbyRequest>>(100);
    fromPlayers.emplace(playerName,fromPlayerQueue);

    toLobby->push({LobbyRequestType::JOIN_LOBBY, playerName});

    return LobbyChannels{
        toLobby,
        fromPlayerQueue
    };
}

bool Lobby::start_game() {
    if (players.size() == maxPlayers) {
        active = false;

        admin.startGame(name);

        for (auto& [name, queue] : fromPlayers) {
            LobbyRequest event = {
                LobbyRequestType::START_LOBBY,
                name
            };
            queue->push(event);
        }
        return true;
    } else {
        std::cerr << "Cannot start game, not enough players in lobby: " << name << std::endl;
        return false;
    }
}

void Lobby::stop() {
    active = false;
    toLobby->close();
    for (auto& [_, queue] : fromPlayers) {
        queue->close();
    }
}

Lobby::~Lobby() {}