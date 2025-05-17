#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "common/game.h"
#include "common/player.h"


#include <iostream>
#include <exception>
#include <iostream>

#include <QApplication>
#include <QWidget>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PLAYER_ID 1 // temporal

using namespace SDL2pp;

int main(int argc, char **argv) try {


    QApplication app(argc, argv);
	QtWindow mainWindow = QtWindow(app, "Counter Strike 2D", 640, 480);
	MenuController menuController(mainWindow);
	//MainView mainView = MainView(app);
	//mainView.run();

	app.exec();


	Window window = GameView::createWindow();
	Renderer renderer = GameView::createRenderer(window);
	Game game(10, 10);
	game.addPlayer("clientplayer", PLAYER_ID);
	GameView gameView = GameView(window, renderer, game, PLAYER_ID);
	GameController gameController = GameController(gameView, game, PLAYER_ID);

	uint32_t lastTime = 0;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);
		while (!gameController.actionQueueIsEmpty()) {
			Action action = gameController.actionQueuePop();
			sender.sendAction(action);
		}
	}

	return 0;
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
