#include "admin.h"
#include "clientHandler.h"
#include "lobby.h"
#include "gameLoop.h"

Admin::Admin() : mtx(), lobbies(), games(), handlers() {}

void Admin::stop() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& pair : lobbies) {
        pair.second->stop();
        pair.second->join();
    }
    for (auto& pair : handlers) {
        pair.second->stop();
        pair.second->join();
    }
    for (auto& pair : games) {
        pair.second->stop();
        pair.second->join();
    }

    lobbies.clear();
    handlers.clear();
    games.clear();
    std::cout << "Admin stopped." << std::endl;
}

void Admin::createLobby(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    if (lobbies.find(name) != lobbies.end()) {
        throw std::runtime_error("Lobby already exists");
    }
    lobbies[name] = std::make_shared<Lobby>(name, *this);
    lobbies[name]->start();
}

LobbyChannels Admin::joinLobby(const std::string& name, const std::string& clientName, Protocol& protocol) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it == lobbies.end()) {
        throw std::runtime_error("Lobby not found");
    }
    
    return it->second->add_player(protocol, clientName);
}

std::vector<std::string> Admin::listLobbies() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> lobbyNames;
    for (const auto& pair : lobbies) {
        lobbyNames.push_back(pair.first);
    }
    return lobbyNames;
}

void Admin::removeLobby(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it != lobbies.end()) {
        lobbies.erase(it);
    }
}

void Admin::registerHandler(const std::string& clientName, std::shared_ptr<ClientHandler> handler) {
    std::lock_guard<std::mutex> lock(mtx);
    handlers[clientName] = std::move(handler);
}

void Admin::removeHandler(const std::string& clientName) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = handlers.find(clientName);
    if (it != handlers.end()) {
        handlers.erase(it);
    }
}

void Admin::startGame(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it != lobbies.end()) {
        games[name] = std::make_shared<GameLoop>(name, *this);
        games[name]->start();
    }
}

GameChannels Admin::joinGame(const std::string& gameName, const std::string& clientName, Protocol& protocol) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = games.find(gameName);
    if (it == games.end()) {
        throw std::runtime_error("Game not found");
    }

    return it->second->add_player(protocol, clientName);
}

void Admin::endGame(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto gameIt = games.find(name);
    if (gameIt != games.end()) {
        gameIt->second->stop();
        gameIt->second->join();
        games.erase(gameIt);
    }
}
