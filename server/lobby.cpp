#include "lobby.h"

Lobby::Lobby(const std::string& name) : name(name), maxPlayers(4), eventQueue(100) {}

void Lobby::run() {
    while (maxPlayers < players.size()) {
        // la idea aca seria que el lobby se mantenga activo hasta que se llene para proximamente ir enviando data sobre la gente del lobby
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool Lobby::add_player(Protocol&& player, const std::string& playerName) {
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
        return false;
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

    
    std::cout << "Player " << playerName << " joined the lobby." << std::endl;

    if (players.size() == maxPlayers) {
        return true;
    }
    return false;
}

void Lobby::remove_player(const std::string& playerName) {
    auto it = players.find(playerName);
    if (it != players.end()) {
        players.erase(it);
        std::cout << "Player " << playerName << " left the lobby." << std::endl;
    } else {
        std::cerr << "Player " << playerName << " not found in the lobby." << std::endl;
    }
}

Lobby::~Lobby() {}