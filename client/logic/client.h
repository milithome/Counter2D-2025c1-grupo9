#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <exception>

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
#include "client/clientSenderLoop.h"
#include "client/clientReceiverLoop.h"

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace SDL2pp;

#include <variant>
#include <atomic>
#include <memory>

class Client {
private:
    std::string clientName;
    std::unique_ptr<RecvLoop> receiver;
    std::unique_ptr<SendLoop> sender;
    bool started;

    std::vector<std::string> players;
	QPoint w_pos_when_started;
    std::unique_ptr<QtWindow> menuWindow;
    QTimer* timer;
    std::unique_ptr<MenuController> menuController;

    std::unique_ptr<GameView> gameView;
    std::unique_ptr<GameController> gameController;

    void prepare_Queues(std::string name_server, int port);
    void SDL_start();
    void connect_Q_object();
    void recv_from_action(Response msg);

    void menu_action_list();
    void menu_action_join();
    void menu_action_leave();
    void menu_action_state_lobby();
    void menu_action_start_game();

    void game_not_started();
    void game_started();
    void game_being_prepared();
    void game_running();

    void update_sdl_ticks();
    void send_action_to_queue();

    void game_ended();


public:
    Client(std::string clientName, std::string name_server, int port);
    void run();
    
};

#endif