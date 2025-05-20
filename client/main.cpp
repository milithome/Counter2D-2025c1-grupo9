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

#include <variant>


int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }
	// TODO: Debera haber un hilo encargado de recibir mensajes del server tanto durante la partida como cuando
	// el cliente se encuentre en un lobby, y de alguna forma se debe comunicar con MenuController y GameController
	// para actualizar las views cuando corresponden (en el caso de GameController tambien hay que checkear q el
	// cliente se encuentre sincronizado con el server)
	bool partida_iniciada = false;
	Socket serverSocket(NAME_SERVER, PORT);
	Protocol protocol(std::move(serverSocket));

	
	Queue<Response> recv_queue(100);
	Queue<std::shared_ptr<MessageEvent>> send_queue(100);

	RecvLoop receiver(protocol, recv_queue);
	SendLoop sender(protocol, send_queue);

	receiver.start();
	sender.start();

	std::string clientName = argv[1];
	protocol.send_name(clientName);

	SDL sdl(SDL_INIT_VIDEO);

    QApplication app(argc, argv);
	QtWindow menuWindow = QtWindow(app, "Counter Strike 2D", 640, 480);
	MenuController menuController(menuWindow, protocol);


	// Menu loop
    QTimer* timer = new QTimer(&menuController);
    QObject::connect(timer, &QTimer::timeout, &menuController, [&recv_queue, &menuController, &partida_iniciada] {
        Response msg;
        while (recv_queue.try_pop(msg)) {
			switch (msg.type) {
				case LIST: {
					menuController.onPartyListReceived(msg.partidas, msg.message, msg.result);
					break;
				}
				case JOIN: {
					menuController.onJoinPartyResponseReceived(msg.message, msg.result);
					break;
				}
				case LEAVE: {
					menuController.onLeavePartyResponseReceived(msg.message, msg.result);
					break;
				}
				case STATE_LOBBY: {
					menuController.onLobbyPlayersReceived(msg.players, msg.message, msg.result);
					break;
				}
				case START: {
					partida_iniciada = true;
					menuController.onGameStarted();
					break;
				}
				default: {
					break;
				}
			}
        }
    });
    timer->start(0);

	// QObject::connect(&menuController, &MenuController::nuevoEvento, [&send_queue](std::unique_ptr<MessageEvent> message) {
	// 	send_queue.try_push(message);
	// });

	QObject::connect(&menuController, &MenuController::nuevoEvento, [&send_queue](MessageEvent* event) {
		//std::unique_ptr<MessageEvent> msg = std::make_shared<MessageEvent>(event)
		send_queue.try_push(std::shared_ptr<MessageEvent>(event));
	});

	app.exec();
	
	if (!partida_iniciada) {
		protocol.send_disconnect();
		Response msg = protocol.recv_response();
		std::cout << msg.message << std::endl;
		return 0;
	};

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

		while (!gameController.actionQueueIsEmpty()) {
			Action action = gameController.actionQueuePop();
			//ActionEvent actionEvent(action);
			std::shared_ptr<MessageEvent> event = std::make_shared<ActionEvent>(action);
			send_queue.try_push(event);
		}
		Response msg;
        while (recv_queue.try_pop(msg)) {
			switch (msg.type) {
				case STATE: {
					gameController.updateGameState(msg.entities);
					break;
				}
				case FINISH: {
					game.stop();
					break;
				}
				default: {
					break;
				}
			}
		}

		// void main() {
		// 	while (not quit) {
		// 		msj = non_blocking_read_from_keyboard();
		// 		if (msj) {
		// 			sender_q.push(msj);
		// 		}
		// 		msj = receiver_q.try_pop();
		// 		draw(msj);
		// 	}
		// }
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
