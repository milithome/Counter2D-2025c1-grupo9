#include "lobby.h"

Lobby::Lobby(const std::string& name) : name(name), maxPlayers(4) {}

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
    
    players[playerName] = std::move(player);
    
    Response response = {
        Type::JOIN,
        0,
        {},
        {},
        0, // success
        ""
    };
    players[playerName].send_response(response);
    
    if (players.size() == maxPlayers) {
        return true;
    }
    return false;
}

void Lobby::remove_player(const std::string& playerName) {
    
}
Lobby::~Lobby() {}