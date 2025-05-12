#include "admin.h"

Admin::Admin() : lobbies() {}

void Admin::createLobby(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    if (lobbies.find(name) != lobbies.end()) {
        throw std::runtime_error("Lobby already exists");
    }
    lobbies[name] = std::make_shared<Lobby>(name);
}

void Admin::joinLobby(const std::string& name, const std::string& clientName, Protocol&& protocol) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it == lobbies.end()) {
        throw std::runtime_error("Lobby not found");
    }
    
    if (it->second->add_player(std::move(protocol), clientName)) {
        startGame(name);
    } 
}

void Admin::leaveLobby(const std::string& name, const std::string& clientName) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it == lobbies.end()) {
        throw std::runtime_error("Lobby not found");
    }
    it->second->remove_player(clientName);
}

void Admin::listLobbies(std::vector<std::string>& lobbiesList) {
    lobbiesList.clear();
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& lobby : lobbies) {
        lobbiesList.push_back(lobby.first);
    }
}

void Admin::removeLobby(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = lobbies.find(name);
    if (it != lobbies.end()) {
        lobbies.erase(it);
    }
}

void Admin::createHandler(const std::string& clientName, Protocol&& protocol) {
    std::lock_guard<std::mutex> lock(mtx);
    handlers[clientName] = std::make_shared<ClientHandler>(std::move(protocol), clientName, * this);
}

void Admin::startHandler(const std::string& clientName) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = handlers.find(clientName);
    if (it != handlers.end()) {
        it->second->start();
    }
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
        games[name] = std::make_shared<GameLoop>();
        games[name]->start();
    }
}