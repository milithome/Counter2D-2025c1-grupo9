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
    bool pulse_available = false;
    for (int i = 0; i < num_audio_drivers; ++i) {
        const char* driver = SDL_GetAudioDriver(i);
        if (strcmp(driver, "pulse") == 0) {
            pulse_available = true;
            break;
        }
    }

    Uint32 flags = SDL_INIT_VIDEO;
    if (pulse_available)
        flags |= SDL_INIT_AUDIO;

    SDL sdl(flags);

    TTF_Init();

    int argc = 1;
    char* argv[] = {const_cast<char*>(clientName.c_str()), nullptr};
	
    MenuClient menuClient(recv_queue, send_queue, receiver, sender, protocol,  w_pos_when_game_started, argc, argv);

    menuClient.run();

    Map map = Map("../assets/maps/default.yaml");  //modificar cuando este el editor (aca tendria que elegir el cliente que mapa usar)
    
    
    players = menuClient.allPlayers();
    
    if(players.empty()) {
        std::cerr << "No players in the lobby." << std::endl;
        return;
    }

    Game game(map.getMapData().game_map);
    for (size_t i = 0; i < players.size(); i++) {
		game.addPlayer(players[i]);
	}
	GameView gameView = GameView(game, clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()}, map);

    try {
        GameClient game_client(game, map, gameView, players, recv_queue, send_queue, clientName, sdl, pulse_available);
        game_client.run();
    } catch (const std::exception& e) {
        std::cerr << "Excepción atrapada: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Excepción desconocida atrapada." << std::endl;
    }

	TTF_Quit();


    receiver.stop();
	sender.stop();
    recv_queue.close();
	send_queue.close();
	receiver.join();
	sender.join();
}