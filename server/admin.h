#ifndef ADMIN_H
#define ADMIN_H

#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "gameLoop.h"
#include "../common/communication/protocol.h"

class ClientHandler;
class Lobby;
class GameLoop;

class Admin {
public:
    Admin();
    ~Admin();
 
    void stop();

    void createLobby(const std::string& name);
    LobbyChannels joinLobby(const std::string& lobbyName, const std::string& clientName, Protocol& protocol);
    std::vector<std::string> listLobbies();
    void removeLobby(const std::string& name);

    void registerHandler(const std::string& clientName, std::shared_ptr<ClientHandler> handler);
    void removeHandler(const std::string& clientName);

    void startGame(const std::string& name);
    GameChannels joinGame(const std::string& gameName, const std::string& clientName, Protocol& protocol);
    void endGame(const std::string& name);
private:
    std::mutex mtx;
    std::map<std::string, std::shared_ptr<Lobby>> lobbies;
    std::map<std::string, std::shared_ptr<GameLoop>> games;
    std::map<std::string, std::shared_ptr<ClientHandler>> handlers;
};

#endif
