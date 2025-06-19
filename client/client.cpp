#include "client.h"

Client::Client(const std::string name, const char* host, const char* port) : 
    clientName(name),
    protocol(Socket(host, port)),
    recv_queue(100),
    send_queue(100),
    receiver(protocol, recv_queue),
    sender(protocol, send_queue)
{
    receiver.start();
	sender.start();
    //protocol.send_name(clientName);
    // Response r = protocol.recv_response();
    // if (r.result) {
    //     throw std::runtime_error(r.message);
    // }
}

void Client::run(QApplication& app, MenuController& menuController) {
    protocol.send_name(clientName);
    bool looped = false;
    while (true) { // mientras el cliente no haya decidido irse
        MenuClient menuClient(app, menuController, recv_queue, send_queue, receiver, sender, protocol);
        InitialData initialData;
        QObject::connect(&menuClient, &MenuClient::initialDataReceived, &menuClient, [this, &initialData, &app](InitialData data) {
            initialData = data;
            app.quit();
        });

        bool quit = menuClient.run(looped);
        QPoint w_pos_when_game_started = menuClient.getWindowPosition();

        if (quit) {
            break;
        }

        Map map = Map(initialData.data);


        GameView gameView = GameView(clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()}, map, initialData.weaponsInfo, initialData.shop, initialData.players);
        GameClient gameClient(map, gameView, players, recv_queue, send_queue, clientName, initialData, true);

        quit = gameClient.run();

        if (quit) {
            break;
        }
        looped = true;
    }
    
    send_queue.push(std::make_shared<DisconnectEvent>());

	receiver.join();
	sender.join();

	TTF_Quit();

}