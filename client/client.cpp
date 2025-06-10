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

    int num_audio_drivers = SDL_GetNumAudioDrivers();
    // bool pulse_available = false;
    // for (int i = 0; i < num_audio_drivers; ++i) {
    //     const char* driver = SDL_GetAudioDriver(i);
    //     if (strcmp(driver, "pulse") == 0) {
    //         pulse_available = true;
    //         break;
    //     }
    // }

    Uint32 flags = SDL_INIT_VIDEO;
    if (num_audio_drivers > 0)
        flags |= SDL_INIT_AUDIO;


    SDL sdl(flags);

    TTF_Init();

    // int argc = 1;
    // char* argv[] = {const_cast<char*>(clientName.c_str()), nullptr};
	
    while (true) { // mientras el cliente no haya decidido irse
        std::cout << "Client::run 1" << std::endl;
        MenuClient menuClient(recv_queue, send_queue, receiver, sender, protocol,  w_pos_when_game_started);

        std::cout << "Client::run 2" << std::endl;
        bool quit = menuClient.run();

        if (quit) {
            break;
        }

        Map map = Map("../assets/maps/big.yaml");  // modificar cuando este el editor (aca tendria que elegir el cliente que mapa usar)
        
        
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
        GameClient gameClient(game, map, gameView, players, recv_queue, send_queue, clientName, sdl, num_audio_drivers > 0);

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