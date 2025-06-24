#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/messages/message_event.h"
#include "client/controllers/messages/action_event.h"
#include "common/communication/protocol.h"
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"
#include "client/menuClient.h"

#include <iostream>
#include <exception>
#include <vector>

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NAME_SERVER "localhost"
#define PORT "12345"

using namespace SDL2pp;

#include <string>

#include <string>

class GameClient {
public:
    GameClient(
        Map& map,
        GameView& gameView,
        std::vector<std::string> players,
        Queue<Response>& recv_queue, 
        Queue<std::shared_ptr<MessageEvent>>& send_queue,
        std::string const clientName,
        InitialData data,
        bool audio_available
    );

    bool run();

private:
    GameController gameController; 
    Map& map;
    GameView& gameView;
    std::vector<std::string> players;
    Queue<Response>& recv_queue;
    Queue<std::shared_ptr<MessageEvent>>& send_queue;

};

#endif // GAMECLIENT_H