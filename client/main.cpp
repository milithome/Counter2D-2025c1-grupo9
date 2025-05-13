#include "client/views/game_view.h"
#include "client/controllers/game_controller.h"
#include "common/game.h"
#include "common/player.h"


#include <iostream>
#include <exception>

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PLAYER_ID 1 // temporal

using namespace SDL2pp;

int main() try {
	SDL sdl(SDL_INIT_VIDEO);

	Window window("Counter Strike 2D",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_RESIZABLE);

	Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
	renderer.SetDrawColor(0, 0, 0, 255);
	
	Game game(10, 10);
	game.addPlayer("clientplayer", PLAYER_ID);
	game.addPlayer("player2", 2);
	GameView gameView = GameView(window, renderer, game, PLAYER_ID);
	GameController gameController = GameController(gameView, game, PLAYER_ID);
	uint32_t lastTime = 0;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);
	}

	return 0;
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
