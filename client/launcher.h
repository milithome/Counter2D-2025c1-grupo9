#ifndef LAUNCHER_H
#define LAUNCHER_H

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

class Launcher {
private:
    SDL sdl;

public:
    // Constructor
    SDL initSDL() {
        int num_audio_drivers = SDL_GetNumAudioDrivers();

        Uint32 flags = SDL_INIT_VIDEO;
        if (num_audio_drivers > 0)
            flags |= SDL_INIT_AUDIO;


        return SDL(flags);
    }

    Launcher() : sdl(initSDL()) {
        TTF_Init();
    };
    ~Launcher() {
        TTF_Quit();
    };

    void run() {
        int argc = 0;
        char** argv = nullptr;
        QApplication app(argc, argv); // Esto deberia estar en otra parte pero qt esta atado con alambre
        QEventLoop waitForConnection;
        qputenv("QT_QPA_PLATFORM", "xcb");
        QtWindow menuWindow("Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT); // Esto deberia estar en otra parte pero qt esta atado con alambre
        MenuController menuController(menuWindow); // Esto deberia estar en otra parte pero qt esta atado con alambre
        std::unique_ptr<Client> client = nullptr;

        QObject::connect(&menuController, &MenuController::connectRequest, [&](const std::string& name, const std::string& addr, const std::string& port) {
            try {
                client = std::make_unique<Client>(name, addr.c_str(), port.c_str());
                menuController.onConnectionRequestResponseReceived("Exito", 0);
                waitForConnection.quit();

            } catch (...) {
                menuController.onConnectionRequestResponseReceived("Conexión fallida", 1);
            }});
        
        waitForConnection.exec();
        client->run(app, menuController);
        /*
        bool quit;
        while (true) {
            std::vector<std::string> players;
            QTimer* timer = new QTimer(nullptr);
            QtWindow menuWindow("Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT); // Esto deberia estar en otra parte pero qt esta atado con alambre
            MenuController menuController(menuWindow); // Esto deberia estar en otra parte pero qt esta atado con alambre
            std::string clientName;
            Queue<Response> recv_queue(100);
            Queue<std::shared_ptr<MessageEvent>> send_queue(100);
            std::shared_ptr<RecvLoop> receiver = nullptr;
            std::shared_ptr<RecvLoop> sender = nullptr;
            std::shared_ptr<Protocol> protocol = nullptr;
            
            QObject::connect(&menuController, &MenuController::connectRequest, [&](const std::string& name, const std::string& addr, const std::string& port) {
                try {
                    clientName = name;
                    // Protocol protocol(Socket(addr.c_str(), port.c_str()));
                    protocol = std::make_shared<Protocol>(Socket(addr.c_str(), port.c_str()));
                    receiver = std::make_shared<RecvLoop>(protocol, recv_queue);
                    sender = std::make_shared<SendLoop>(protocol, send_queue);
                    // receiver->start();
                    // sender->start();
                    protocol.send_name(clientName);

                    // MenuClient menuClient(app, menuController, recv_queue, send_queue, receiver, sender, protocol);
                    // menuClient.run();

                    
                    // Iniciar el bucle de eventos del menú
                    quit = true;
                    QObject::connect(timer, &QTimer::timeout, timer, [&]() {
                        Response msg;
                        while (recv_queue.try_pop(msg)) {
                            switch (msg.type) {
                                case LIST: {
                                    LobbyList data = std::get<LobbyList>(msg.data);
                                    menuController.onPartyListReceived(data.lobbies, msg.message, msg.result);
                                    break;
                                }
                                case JOIN: {
                                    menuController.onJoinPartyResponseReceived(msg.message, msg.result);
                                    break;
                                }
                                case CREATE: {
                                    menuController.onCreatePartyResponseReceived(msg.message, msg.result);
                                    break;
                                }
                                case LEAVE: {
                                    menuController.onLeavePartyResponseReceived(msg.message, msg.result);
                                    break;
                                }
                                case STATE_LOBBY: {
                                    StateLobby data = std::get<StateLobby>(msg.data);
                                    players = data.players;
                                    menuController.onLobbyPlayersReceived(players, msg.message, msg.result);
                                    break;
                                }
                                case LOBBY_READY: {
                                    menuController.onLobbyReady();
                                    break;
                                }
                                case START: {
                                    quit = false;
                                    menuController.onGameStarted();
                                    app.quit();
                                    break;
                                }
                                default: {
                                    break;
                                }
                            }
                        }
                    });
                    timer->start(16); // limita el menu a 60fps

                    // Conectar señales y slots
                    QObject::connect(&menuController, &MenuController::nuevoEvento, [this, &send_queue](std::shared_ptr<MessageEvent> event) {
                        send_queue.try_push(event);
                    });
                } catch (...) {
                    std::cout << "No se pudo conectar al server" << std::endl;
                }
            });
            app.exec();
            delete timer;

            if (quit) break;

            QPoint w_pos_when_game_started = menuWindow.getPosition();
            Map map = Map("../assets/maps/big.yaml");  // modificar cuando este el editor (aca tendria que elegir el cliente que mapa usar)
            
            
            // std::vector<std::string> players = menuClient.allPlayers();
            
            if (players.empty()) {
                std::cerr << "No players in the lobby." << std::endl;
                return;
            }

            Game game(map.getMapData().game_map);
            for (size_t i = 0; i < players.size(); i++) {
                game.addPlayer(players[i]);
            }

            GameView gameView = GameView(game, clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()}, map);
            GameClient gameClient(game, map, gameView, players, recv_queue, send_queue, clientName, true);

            quit = gameClient.run();

            if (quit) break;

        }
        send_queue.push(std::make_shared<DisconnectEvent>());

        receiver->join();
        sender->join();
        // QtWindow menuWindow("Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT); // Esto deberia estar en otra parte pero qt esta atado con alambre
        // MenuController menuController(menuWindow); // Esto deberia estar en otra parte pero qt esta atado con alambre

        // QObject::connect(&menuController, &MenuController::connectRequest, [&app, &menuController](const std::string& name, const std::string& addr, const std::string& port) {
        //     try {
        //         Client client(name, addr.c_str(), port.c_str());
        //         client.run(app, menuController);
        //     } catch (...) {
        //         std::cout << "No se pudo conectar al server" << std::endl;
        //     }
        // });

        // app.exec();
        */
    };


};

#endif