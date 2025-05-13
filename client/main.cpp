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

using namespace SDL2pp;

int main() try {
	// Initialize SDL library
	SDL sdl(SDL_INIT_VIDEO);

	// Create main window: 640x480 dimensions, resizable, "SDL2pp demo" title
	Window window("SDL2pp demo",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_RESIZABLE);

	// Create accelerated video renderer with default driver
	Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
	renderer.SetDrawColor(0, 0, 0, 255); // Fondo negro
	
	Game game(10, 10);
	Player player("hola", 1);
	GameView gameView = GameView(window, renderer, game, player);
	GameController gameController = GameController(gameView, game, player);
	float lastTime = 0.0f;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);
	}

	// Here all resources are automatically released and library deinitialized
	return 0;
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return 1;
}
