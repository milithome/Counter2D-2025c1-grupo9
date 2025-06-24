#ifndef MENUCLIENT_H
#define MENUCLIENT_H

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

using namespace SDL2pp;

#include <string>

#include <iostream>
#include <vector>
#include <string>

class MenuClient : public QObject {
    Q_OBJECT
private:
    bool partida_iniciada = false;
    Queue<Response>& recv_queue;
    Queue<std::shared_ptr<MessageEvent>>& send_queue;
    RecvLoop& receiver;
    SendLoop& sender;
    Protocol& protocol;
    std::vector<std::string> players;
    QApplication& app;
    MenuController& menuController;

public:
    MenuClient(
         QApplication& app,
         MenuController& menuController,
         Queue<Response>& recv_queue, 
         Queue<std::shared_ptr<MessageEvent>>& send_queue, 
         RecvLoop& receiver, 
         SendLoop& sender, 
         Protocol& protocol);
    
    bool run(bool looped);

    bool isGameStarted() const {
        return partida_iniciada;
    }
    QPoint getWindowPosition() {
        return menuController.getWindowPosition();
    }

    std::vector<std::string> allPlayers();

signals:
    void initialDataReceived(InitialData data);
};

#endif // MENUCLIENT_H