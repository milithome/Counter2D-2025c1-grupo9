#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <map>
#include "thread.h"
#include "../common/protocol.h"
#include "queue.h"

enum class LobbyEventType {
    LEAVE,
};

struct LobbyEvent {
    LobbyEventType type;
    std::string playerName;
};

class Lobby : public Thread {
public:
    explicit Lobby(const std::string& name);
    void run() override;
    ~Lobby() override;

    bool add_player(Protocol&& player, const std::string& playerName);
    void remove_player(const std::string& playerName);
private:
    std::string name;
    std::map<std::string, Protocol> players;
    size_t maxPlayers;
    Queue<LobbyEvent> eventQueue;
};
#endif