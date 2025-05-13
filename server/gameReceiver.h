#ifndef GAME_RECEIVER_H
#define GAME_RECEIVER_H

#include "thread.h"
#include "../common/communication/protocol.h"
#include "queue.h"

class GameReceiver : public Thread {
public:
    explicit GameReceiver(Protocol& player,const std::string& playerName, Queue<ActionEvent>& eventQueue);
    virtual void run() override;
    void stop() override;
    ~GameReceiver();
private:
    bool active;
    Protocol& player;
    std::string playerName;
    Queue<ActionEvent>& eventQueue;
};

#endif