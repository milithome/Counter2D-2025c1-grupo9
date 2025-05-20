#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <map>
#include "common/utilities/thread.h"
#include "../common/communication/protocol.h"
#include "common/utilities/queue.h"
#include "admin.h"
#include "../common/structures.h"

class Lobby : public Thread {
public:
    explicit Lobby(const std::string& name, Admin& admin);
    void run() override;
    ~Lobby() override;

    LobbyChannels add_player(Protocol& player, const std::string& playerName);
    void stop() override;
private:
    std::string name;
    std::map<std::string, Protocol&> players;
    Admin& admin;
    size_t maxPlayers;
    std::shared_ptr<Queue<LobbyEvent>> toLobby;
    std::map<std::string, std::shared_ptr<Queue<LobbyEvent>>> fromPlayers;


    bool active;

    void handle_join_event(const std::string& playerName);
    void handle_leave_event(const std::string& playerName);
    void broadcast_lobby_state();
};
#endif