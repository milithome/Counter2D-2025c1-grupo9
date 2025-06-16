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
}

void Client::run(QApplication& app, MenuController& menuController) {
    protocol.send_name(clientName);
    bool looped = false;
    while (true) { // mientras el cliente no haya decidido irse
        MenuClient menuClient(app, menuController, recv_queue, send_queue, receiver, sender, protocol);

        bool quit = menuClient.run(looped);
        QPoint w_pos_when_game_started = menuClient.getWindowPosition();

        if (quit) {
            break;
        }

        Map map = Map("../assets/maps/big.yaml");  // modificar cuando este el editor (aca tendria que elegir el cliente que mapa usar)
        
        
        // std::vector<std::string> players = menuClient.allPlayers();
        
        // if (players.empty()) {
        //     std::cerr << "No players in the lobby." << std::endl;
        //     break;
        // }

        // Game game(map.getMapData().game_map);
        // for (size_t i = 0; i < players.size(); i++) {
        //     game.addPlayer(players[i]);
        // }


        GameView gameView = GameView(clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()}, map);
        GameClient gameClient(map, gameView, players, recv_queue, send_queue, clientName, true);

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