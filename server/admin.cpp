#include "admin.h"
#include "clientHandler.h"
#include "lobby.h"
#include "gameLoop.h"

Admin::Admin(){}

Admin::~Admin(){}

void Admin::stop() {
    //std::lock_guard<std::mutex> lock(mtx);
    // Paro los hilos
    for (auto& pair : handlers) {
        pair.second->stop();
    }

    for (auto& pair : lobbies) {
        pair.second->stop();
    }

    for (auto& pair : games) {
        pair.second->stop();
    }

    // Espero que paren los hilos
    for (auto& pair : lobbies) {
        pair.second->join();
    }

    for (auto& pair : games) {
        pair.second->join();
    }

    for (auto& pair : handlers) {
        pair.second->join();
    }

    // Limpio los arreglos
    lobbies.clear();
    handlers.clear();
    games.clear();
    removeFinishedGames();
    removeFinishedHandlers();
    removeFinishedLobbies();
}

void Admin::createLobby(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);

    removeFinishedGames();
    removeFinishedLobbies();
    removeFinishedHandlers();
    
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
    completeLobbies.push_back(name);
}

void Admin::registerHandler(const std::string& clientName, std::shared_ptr<ClientHandler> handler) {
    std::lock_guard<std::mutex> lock(mtx);
    
    removeFinishedGames();
    removeFinishedLobbies();
    removeFinishedHandlers();

    handlers[clientName] = std::move(handler);
}

void Admin::removeHandler(const std::string& clientName) {
    std::lock_guard<std::mutex> lock(mtx);
    completeHandlers.push_back(clientName);
}

void Admin::startGame(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);

    removeFinishedGames();
    removeFinishedLobbies();
    removeFinishedHandlers();

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
    completedGames.push_back(name);
}

void Admin::removeFinishedGames() {
    for (const auto& name : completedGames) {
        auto it = games.find(name);
        if (it != games.end()) {
            it->second->stop();
            it->second->join();
            games.erase(it);
        }
    }

    completedGames.clear();
}

void Admin::removeFinishedHandlers() {
    for (const auto& name : completeHandlers) {
        auto it = handlers.find(name);
        if (it != handlers.end()) {
            it->second->stop();
            it->second->join();
            handlers.erase(it);
        }
    }

    completeHandlers.clear();
}

void Admin::removeFinishedLobbies() {
    for (const auto& name : completeLobbies) {
        auto it = lobbies.find(name);
        if (it != lobbies.end()) {
            it->second->stop();
            it->second->join();
            lobbies.erase(it);
        }
    }

    completeLobbies.clear();
}