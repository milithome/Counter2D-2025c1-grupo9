#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <thread.h>

class GameLoop : public Thread {
public:
    GameLoop();
    void run() override;
    ~GameLoop() override;
};
#endif