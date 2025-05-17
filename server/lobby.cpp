#include "lobby.h"

Lobby::Lobby(const std::string& name, Admin& admin) 
    : name(name), players(), admin(admin), handlers(), maxPlayers(4), eventQueue(100), active(true) {}

    void Lobby::run() {
        try {
            while (active) {
                LobbyEvent event;
                if (eventQueue.try_pop(event)) {
                    switch (event.type) {
                        case LobbyEventType::JOIN:
                            handle_join_event(event.playerName);
                            break;
                        case LobbyEventType::LEAVE:
                            handle_leave_event(event.playerName);
                            break;
                    }
    
                    broadcast_lobby_state();
    
                    if (players.size() == maxPlayers) {
                        active = false;
                    }
                
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
            
            for (auto& pair : handlers) {
                pair.second->stop();
            }

            for (auto& pair : handlers) {
                pair.second->join();
            }

            admin.startGame(name,std::move(players));

        } catch (const std::exception& e) {
            std::cerr << "Exception in Lobby::run: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception in Lobby::run" << std::endl;
        }
    }
    
void Lobby::handle_join_event(const std::string& playerName) {
    auto it = players.find(playerName);
    if (it != players.end()) {
        auto handler = std::make_shared<LobbyClientHandler>(it->second, playerName, eventQueue);
        handlers.emplace(playerName, handler);
        handler->start();
    }
    std::cout << "Player " << playerName << " joined the lobby." << std::endl;
}

void Lobby::handle_leave_event(const std::string& playerName) {
    auto it = players.find(playerName);
    if (it != players.end()) {
        Response response = {
            Type::LEAVE,
            0,
            {},
            {},
            0, // success
            ""
        };
        it->second.send_response(response);
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
        pair.second.send_state_lobby(playerNames);
    }
}
    
void Lobby::add_player(Protocol&& player, const std::string& playerName) {
    if (maxPlayers <= players.size()) {
        Response response = {
            Type::JOIN,
            0,
            {},
            {},
            1, // error
            "Lobby is full"
        };
        player.send_response(response);
    }
    
    players.emplace(playerName, std::move(player));
    
    Response response = {
        Type::JOIN,
        0,
        {},
        {},
        0, // success
        ""
    };
    auto it = players.find(playerName);
    if (it != players.end()) {
        it->second.send_response(response);
    }

    eventQueue.push({LobbyEventType::JOIN, playerName});
}

void Lobby::stop() {
    active = false;
    eventQueue.close();
    for (auto& pair : handlers) {
        pair.second->stop();
    }
}

Lobby::~Lobby() {
    std::cout << "Lobby destructor called." << std::endl;
}