#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "common/communication/protocol.h"
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"
#include "client/menuClient.h"
#include "client/gameClient.h"
#include "client/client.h"

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
        // Nota: qt esta atado con alambre
        int argc = 0;
        char** argv = nullptr;
        QApplication app(argc, argv);
        qputenv("QT_QPA_PLATFORM", "xcb");
        QtWindow menuWindow("Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT);
        MenuController menuController(menuWindow); 


        QEventLoop waitForConnection;
        QEventLoop waitForServerResponse;
        bool quit = false;
        QObject::connect(&menuWindow, &QtWindow::windowClosed, [&]() {
            quit = true;
            waitForConnection.quit();
            waitForServerResponse.quit();
        });

        std::unique_ptr<Client> client = nullptr;
        QObject::connect(&menuController, &MenuController::connectRequest, [&](const std::string& name, const std::string& addr, const std::string& port) {
            try {
                client = std::make_unique<Client>(name, addr.c_str(), port.c_str());
                waitForConnection.quit();
            } catch (...) {
                menuController.onConnectionRequestResponseReceived("Conexión fallida: no se encontro un servidor con esa dirección y puerto", 1);
            }
        });

        bool connected = false;
        while (!connected) {
            waitForConnection.exec();
            if (quit) return;
            if (client) {
                QTimer *timer = new QTimer(nullptr);
                QObject::connect(timer, &QTimer::timeout, timer, [&]() {
                    try {
                        bool received = client->receiveConnectionResponse();
                        if (received) {
                            menuController.onConnectionRequestResponseReceived("Exito", 0);
                            connected = true;
                            waitForServerResponse.quit();
                        }
                    } catch (const std::runtime_error& e) {
                        menuController.onConnectionRequestResponseReceived(e.what(), 1);
                        waitForServerResponse.quit();
                        client = nullptr;
                    }
                });
                timer->start(16);
                waitForServerResponse.exec();
                delete timer;
            }
        }
        client->run(app, menuController);
    };
};

#endif