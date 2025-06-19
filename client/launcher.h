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
        // Nota: qt esta atado con un alambre
        int argc = 0;
        char** argv = nullptr;
        QApplication app(argc, argv);
        QEventLoop waitForConnection;
        qputenv("QT_QPA_PLATFORM", "xcb");
        QtWindow menuWindow("Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT);
        MenuController menuController(menuWindow); 
        std::unique_ptr<Client> client = nullptr;

        QObject::connect(&menuController, &MenuController::connectRequest, [&](const std::string& name, const std::string& addr, const std::string& port) {
            try {
                client = std::make_unique<Client>(name, addr.c_str(), port.c_str());
                menuController.onConnectionRequestResponseReceived("Exito", 0);
                waitForConnection.quit();

            } catch (...) {
                menuController.onConnectionRequestResponseReceived("Conexión fallida", 1);
            }
        });
        QObject::connect(&menuWindow, &QtWindow::windowClosed, [&]() {
            waitForConnection.quit();
        });

        waitForConnection.exec();
        if (client) {
            client->run(app, menuController);
        }
    };


};

#endif