#ifndef ADMIN_H
#define ADMIN_H

#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "lobby.h"
#include "game_loop.h"
#include "clientHandler.h"

class Admin {
public:
    void createLobby(const std::string& name);
    void joinLobby(const std::string& name, const std::string& clientName, Protocol&& protocol);
    void leaveLobby(const std::string& name, const std::string& clientName);
    void listLobbies(std::vector<std::string>& lobbies);
    void removeLobby(const std::string& name);

    void createHandler(const std::string& clientName, Protocol&& protocol);
    void startHandler(const std::string& clientName);
    void removeHandler(const std::string& clientName);
private:
    std::mutex mtx;
    std::map<std::string, std::shared_ptr<Lobby>> lobbies;
    std::map<std::string, std::shared_ptr<GameLoop>> games;
    std::map<std::string, std::shared_ptr<ClientHandler>> handlers;

    void startGame(const std::string& name);

};

#endif
