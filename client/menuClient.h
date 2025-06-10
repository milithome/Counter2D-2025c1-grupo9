#ifndef MENUCLIENT_H
#define MENUCLIENT_H

#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "client/controllers/action_event.h"
#include "common/game.h"
#include "common/player.h"
#include "common/communication/protocol.h"
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"

#include <iostream>
#include <exception>
#include <vector>

#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QApplication>
#include <QString>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NAME_SERVER "localhost"
#define PORT "12345"

using namespace SDL2pp;

#include <string>

#include <iostream>
#include <vector>
#include <string>

class MenuClient {
private:
    bool partida_iniciada;
    Queue<Response>& recv_queue;
    Queue<std::shared_ptr<MessageEvent>>& send_queue;
    RecvLoop& receiver;
    SendLoop& sender;
    Protocol& protocol;
    QApplication app;
    QApplication createApp() {
        int argc = 0;
        char** argv = nullptr;
        return QApplication(argc, argv);
    }
    // QtWindow menuWindow;
    // MenuController menuController;
    //QTimer* timer;
    std::vector<std::string> players;
    QPoint& w_pos_when_game_started;

public:
    MenuClient(Queue<Response>& recv_queue, 
         Queue<std::shared_ptr<MessageEvent>>& send_queue, 
         RecvLoop& receiver, 
         SendLoop& sender, Protocol& protocol, QPoint& w_pos_when_game_started);
    
    bool run();

    bool isGameStarted() const {
        return (true == partida_iniciada);
    }

    std::vector<std::string> allPlayers();
    
};

#endif // MENUCLIENT_H