#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "thread.h"

class GameLoop : public Thread {
public:
    GameLoop();
    void run() override;
    ~GameLoop() override;

    void stop() override;
private:
    bool active = true;
};
#endif