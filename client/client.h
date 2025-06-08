#ifndef CLIENT_H
#define CLIENT_H

#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "client/controllers/disconnect_event.h"
#include "client/controllers/action_event.h"
#include "common/game.h"
#include "common/player.h"
#include "common/communication/protocol.h"
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"
#include "client/menuClient.h"
#include "client/gameClient.h"

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

class Client {
private:
    std::string clientName;
    bool partida_iniciada;
    Protocol protocol;
    Queue<Response> recv_queue;
    Queue<std::shared_ptr<MessageEvent>> send_queue;
    RecvLoop receiver;
    SendLoop sender;
    std::vector<std::string> players;
    QPoint w_pos_when_game_started;
    //Map map_selected ()  se agarraria en una ventana de los mapas en la memoria del editor

public:
    // Constructor
    Client(const std::string name, const char* host, const char* port);

    void run();
};

#endif // CLIENT_H