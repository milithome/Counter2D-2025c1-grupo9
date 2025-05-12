#ifndef LOBBY_CLIENT_HANDLER_H
#define LOBBY_CLIENT_HANDLER_H

#include "thread.h"
#include "../common/protocol.h"
#include "queue.h"
#include "../common/structures.h"

class LobbyClientHandler : public Thread {
public:
    explicit LobbyClientHandler(Protocol& player, const std::string& playerName, Queue<LobbyEvent>& eventQueue);
    void run() override;
    void stop() override;
    ~LobbyClientHandler() override;
private:
    Protocol& player;
    std::string playerName;
    Queue<LobbyEvent>& eventQueue;
    bool active;
};

#endif
