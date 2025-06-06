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
#include "client/receiver&sender/clientReceiverLoop.h"
#include "client/receiver&sender/clientSenderLoop.h"
#include "client/client.h"


#include <iostream>
#include <exception>

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

#include <variant>


// void game_run(std::string clientName);

int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }
	Client client = Client(argv[1], NAME_SERVER, PORT);
	client.run();

	return 0;

} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}

// // main reducido para testear
// void game_run(std::string clientName) {
// 	SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
// 	TTF_Init();
	
// 	Map map = Map("../assets/maps/default.yaml");
// 	Game game(map.getMapData().game_map);
// 	game.addPlayer(clientName);
// 	//GameView(Game& game, const std::string& playerName, SDL_Point window_pos, const std::string& background_path, const std::string& sprite_path, const std::vector<std::vector<uint16_t>>& tiles_map, const std::unordered_map<uint16_t, MapLegendEntry>& legend_tiles);
// 	GameView gameView = GameView(game, clientName, SDL_Point{0, 0}, map);
// 	GameController gameController = GameController(gameView, game, clientName);

// 	uint32_t lastTime = 0;
// 	while (game.isRunning()) {
// 		uint32_t currentTime = SDL_GetTicks();
// 		float deltaTime = (currentTime - lastTime) / 1000.0f;
// 		lastTime = currentTime;
// 		gameView.update(deltaTime);
// 		gameController.processEvents();
// 		gameController.update(deltaTime);
// 	}

// 	TTF_Quit();
// }