#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "common/game.h"
#include "common/player.h"
#include "common/communication/protocol.h"


#include <iostream>
#include <exception>
#include <iostream>

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PLAYER_ID 1 // temporal

using namespace SDL2pp;

int main(int argc, char **argv) try {

	// TODO: Debera haber un hilo encargado de recibir mensajes del server tanto durante la partida como cuando
	// el cliente se encuentre en un lobby, y de alguna forma se debe comunicar con MenuController y GameController
	// para actualizar las views cuando corresponden (en el caso de GameController tambien hay que checkear q el
	// cliente se encuentre sincronizado con el server)
	bool partida_iniciada = false;
	Socket serverSocket("12345");
	Protocol protocol(std::move(serverSocket));

	SDL sdl(SDL_INIT_VIDEO);

    QApplication app(argc, argv);
	QtWindow menuWindow = QtWindow(app, "Counter Strike 2D", 640, 480);
	MenuController menuController(menuWindow, protocol);

	QObject::connect(&menuController, &MenuController::partidaIniciada, [&partida_iniciada]() {
		partida_iniciada = true;
	});

	// TODO: Esto pasara a ser ejecutado desde otro hilo que contendra a 
	// menuController, para evitar que el loop de Qt se trabe al enviar un mensaje.
	QObject::connect(&menuController, &MenuController::nuevoEvento, [&protocol](const MessageEvent& message) {
		message.send(protocol);
	});

	app.exec();
	if (!partida_iniciada) return 0;

	Game game(10, 10);
	game.addPlayer("clientplayer", PLAYER_ID);

	GameView gameView = GameView(game, PLAYER_ID);
	GameController gameController = GameController(gameView, game, PLAYER_ID);

	uint32_t lastTime = 0;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);

		// TODO: Esto pasara a ser ejecutado desde otro hilo que contendra a 
		// gameController, para evitar que este loop se trabe al enviar un mensaje.
		while (!gameController.actionQueueIsEmpty()) {
			Action action = gameController.actionQueuePop();
			protocol.send_accion(action);
		}
	}

	return 0;
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
