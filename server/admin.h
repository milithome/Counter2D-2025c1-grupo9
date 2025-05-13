#ifndef ADMIN_H
#define ADMIN_H

#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "game_loop.h"
#include "../common/protocol.h"

class ClientHandler;
class Lobby;


class Admin {
public:
    Admin();
 
    void stop();

    void createLobby(const std::string& name);
    void joinLobby(const std::string& name, const std::string& clientName, Protocol&& protocol);
    std::vector<std::string> listLobbies();
    void removeLobby(const std::string& name);

    void createHandler(const std::string& clientName, Protocol&& protocol);
    void startHandler(const std::string& clientName);
    void removeHandler(const std::string& clientName);

    void startGame(const std::string& name);
private:
    std::mutex mtx;
    std::map<std::string, std::shared_ptr<Lobby>> lobbies;
    std::map<std::string, std::shared_ptr<GameLoop>> games;
    std::map<std::string, std::shared_ptr<ClientHandler>> handlers;
};

#endif
