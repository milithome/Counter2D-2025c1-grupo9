#include "client.h"

Client::Client(const std::string name, const char* host, const char* port):
    clientName(name),
    partida_iniciada(false),
    protocol(Socket(host, port)),
    recv_queue(100),
    send_queue(100),
    receiver(protocol, recv_queue),
    sender(protocol, send_queue)
{
    receiver.start();
	sender.start();
}

void Client::run() {
    // Enviar el nombre del cliente al servidor
    protocol.send_name(clientName);

    SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();

    int argc = 1;
    char* argv[] = {const_cast<char*>(clientName.c_str()), nullptr};
	
    while (true) { // mientras el cliente no haya decidido irse

        MenuClient menuClient(recv_queue, send_queue, receiver, sender, protocol,  w_pos_when_game_started, argc, argv);

        bool quit = menuClient.run();

        if (quit) {
            break;
        }

        Map map = Map("../assets/maps/default.yaml");  // modificar cuando este el editor (aca tendria que elegir el cliente que mapa usar)
        
        
        players = menuClient.allPlayers();
        
        if (players.empty()) {
            std::cerr << "No players in the lobby." << std::endl;
            break;
        }

        Game game(map.getMapData().game_map);
        for (size_t i = 0; i < players.size(); i++) {
            game.addPlayer(players[i]);
        }
        GameView gameView = GameView(game, clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()}, map);

        GameClient gameClient(game, map, gameView, players, recv_queue, send_queue, clientName, sdl);

        quit = gameClient.run();

        if (quit) {
            break;
        }
    }
    send_queue.push(std::make_shared<DisconnectEvent>());

	receiver.join();
	sender.join();

	TTF_Quit();

}