#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <map>
#include "thread.h"
#include "queue.h"
#include "../common/communication/protocol.h"
#include "gameReceiver.h"
#include "../common/game.h"

class Admin;

class GameLoop : public Thread {
public:
    explicit GameLoop(std::string name, Admin& admin, std::map<std::string, Protocol>&& players);
    void run() override;
    ~GameLoop() override;
    void stop() override;
private:
    std::string name;
    Admin& admin;
    std::map<std::string, Protocol> players;
    Queue<ActionEvent> eventQueue;
    std::map<std::string, std::shared_ptr<GameReceiver>> handlers;
    bool active;

    void broadcast_game_state(std::vector<Entity>& entities);
};
#endif