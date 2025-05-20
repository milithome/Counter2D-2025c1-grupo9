#include "client/views/game_view.h"
#include "client/views/main_view.h"
#include "client/views/qtwindow.h"
#include "client/controllers/game_controller.h"
#include "client/controllers/menu_controller.h"
#include "client/controllers/message_event.h"
#include "common/game.h"
#include "common/player.h"
#include "common/communication/protocol.h"
#include "client/clientSenderLoop.h"
#include "client/clientReceiverLoop.h"


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
#define NAME_SERVER "localhost"
#define PLAYER_NAME "client"
#define PORT "12345"

using namespace SDL2pp;

int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }

	Queue<std::string> recv_queue(100);
	Queue<std::string> send_queue(100);

	clientReceiverLoop receiver(protocol, recv_queue);
	clientSenderLoop sender(protocol, send_queue);

	receiver.start();
	sender.start();

	// TODO: Debera haber un hilo encargado de recibir mensajes del server tanto durante la partida como cuando
	// el cliente se encuentre en un lobby, y de alguna forma se debe comunicar con MenuController y GameController
	// para actualizar las views cuando corresponden (en el caso de GameController tambien hay que checkear q el
	// cliente se encuentre sincronizado con el server)
	bool partida_iniciada = false;
	Socket serverSocket(NAME_SERVER, PORT);
	Protocol protocol(std::move(serverSocket));
	std::string clientName = argv[1];
	protocol.send_name(clientName);

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
		send_queue.try_push(message);
		//message.send(protocol);
	});

	app.exec();
	
	if (!partida_iniciada) return 0;

	Game game(10, 10);
	game.addPlayer(PLAYER_NAME);

	GameView gameView = GameView(game, PLAYER_NAME);
	GameController gameController = GameController(gameView, game, PLAYER_NAME);

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

			protocol.send_action(action);
		}

		void main() {
			while (not quit) {
				msj = non_blocking_read_from_keyboard();
				if (msj) {
					sender_q.push(msj);
				}
				msj = receiver_q.try_pop();
				draw(msj);
			}
		}
	}

	receiver.stop();
	sender.stop();
	receiver.join();
	sender.join();

	return 0;
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}
