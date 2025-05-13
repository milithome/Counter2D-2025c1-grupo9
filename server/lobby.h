#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <map>
#include "thread.h"
#include "../common/communication/protocol.h"
#include "queue.h"
#include "lobbyClientHandler.h"
#include "admin.h"
#include "../common/structures.h"

class Lobby : public Thread {
public:
    explicit Lobby(const std::string& name, Admin& admin);
    void run() override;
    ~Lobby() override;

    void add_player(Protocol&& player, const std::string& playerName);
    void stop() override;
private:
    std::string name;
    std::map<std::string, Protocol> players;
    Admin& admin;
    std::map<std::string, std::shared_ptr<LobbyClientHandler>> handlers;
    size_t maxPlayers;
    Queue<LobbyEvent> eventQueue;
    bool active;

    void handle_join_event(const std::string& playerName);
    void handle_leave_event(const std::string& playerName);
    void broadcast_lobby_state();
};
#endif