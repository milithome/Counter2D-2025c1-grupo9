#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <map>
#include "common/utilities/thread.h"
#include "common/utilities/queue.h"
#include "../common/communication/protocol.h"
#include "../common/game.h"

class Admin;

class GameLoop : public Thread {
public:
    explicit GameLoop(std::string name, Admin& admin);
    void run() override;

    GameChannels add_player(Protocol& protocol, const std::string& name);

    ~GameLoop() override;
    void stop() override;
private:
    std::string name;
    Admin& admin;
    std::map<std::string, Protocol&> players;
    std::shared_ptr<Queue<ActionRequest>> toGame;
    std::map<std::string, std::shared_ptr<Queue<ActionRequest>>> fromPlayers;
    bool active;
    Map map;
    Game game;

    void broadcast_initial_data(const MapData& mapData);
    void broadcast_game_state(StateGame& state);
};
#endif