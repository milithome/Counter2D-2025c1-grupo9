#ifndef SDLCLIENT_H
#define SDLCLIENT_H

#include "client/logic/client.h"
#include "client/views/game_view.h"
#include "client/views/main_view.h"

#include <SDL2/SDL.h>
#include <string>

#include <variant>
#include <atomic>

class SDLClient {
public:
    SDLClient(Queue<Response>& rQueue, Queue<std::shared_ptr<MessageEvent>>& sQueue);
    ~SDLClient();

    bool initialize(const std::string& windowTitle, int width, int height);
    void handleEvents();
    void render();
    void cleanUp();

    bool isRunning() const;

private:
    Queue<Response>& recv_queue;
    Queue<std::shared_ptr<MessageEvent>>& send_queue;
    GameView gameView;
    GameController gameController;
};

#endif // SDLCLIENT_H