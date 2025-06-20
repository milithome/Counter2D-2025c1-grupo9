#include "client/launcher.h"



void game_run(std::string clientName);

int main(int argc, char **argv) try {
	(void)argc;
	(void)argv;

	Launcher launcher;
	launcher.run();

	return 0;

} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}

/*
void game_run(std::string clientName) {
	SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	TTF_Init();
	
	Map map = Map("../assets/maps/default.yaml");
	Game game(map.getMapData().game_map);
	game.addPlayer(clientName);
	game.addPlayer("test");
	GameView gameView = GameView(game, clientName, SDL_Point{0, 0}, map);
	GameController gameController = GameController(gameView, game, clientName);

	uint32_t lastTime = 0;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);
		gameController.processEvents();
		gameController.update(deltaTime);
	}

	TTF_Quit();
}
*/