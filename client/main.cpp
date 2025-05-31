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


void game_run(std::string clientName);

int main(int argc, char **argv) try {
	if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_name>" << std::endl;
        return 1;
    }
	std::string clientName = argv[1];
	game_run(clientName);
	return 0;


/*
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

	SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
    QApplication app(argc, argv);
	QtWindow menuWindow = QtWindow(app, "Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT);
	MenuController menuController(menuWindow, protocol);
	std::vector<std::string> players;
	QPoint w_pos_when_game_started;

	// Menu loop
    QTimer* timer = new QTimer(&menuController);
    QObject::connect(timer, &QTimer::timeout, &menuController, [&recv_queue, &w_pos_when_game_started, &menuController, &menuWindow, &partida_iniciada, &players] {
        Response msg;
        while (recv_queue.try_pop(msg)) {
			switch (msg.type) {
				case LIST: {
					LobbyList data = std::get<LobbyList>(msg.data);
					menuController.onPartyListReceived(data.lobbies, msg.message, msg.result);
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
					StateLobby data = std::get<StateLobby>(msg.data);
					players = data.players;
					menuController.onLobbyPlayersReceived(players, msg.message, msg.result);
					break;
				}
				case START: {
					partida_iniciada = true;
					w_pos_when_game_started = menuWindow.getPosition();
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


	QObject::connect(&menuController, &MenuController::nuevoEvento, [&send_queue](MessageEvent* event) {
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
	for (size_t i = 0; i < players.size(); i++) {
		game.addPlayer(players[i]);
	}
	GameView gameView = GameView(game, clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()});
	GameController gameController = GameController(gameView, game, clientName);

	uint32_t lastTime = 0;
	while (game.isRunning()) {
		uint32_t currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;
		gameView.update(deltaTime);
		gameController.processEvents();
		gameController.update(deltaTime);

		while (!gameController.actionQueueIsEmpty()) {
			Action action = gameController.actionQueuePop();
			std::shared_ptr<MessageEvent> event = std::make_shared<ActionEvent>(action);
			send_queue.try_push(event);
		}
		Response msg;
        while (recv_queue.try_pop(msg)) {
			switch (msg.type) {
				case STATE: {
					StateGame data = std::get<StateGame>(msg.data);
					gameController.updateGameState(data);
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
	}
	receiver.stop();
	sender.stop();
	receiver.join();
	sender.join();
	recv_queue.close();
	send_queue.close();

	return 0;
*/
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 1;
}

// main reducido para testear
void game_run(std::string clientName) {
	SDL sdl(SDL_INIT_VIDEO);

	Map map = Map("../assets/maps/default.yaml");
	Game game(10, 10);
	game.addPlayer(clientName);
	//GameView(Game& game, const std::string& playerName, SDL_Point window_pos, const std::string& background_path, const std::string& sprite_path, const std::vector<std::vector<uint16_t>>& tiles_map, const std::unordered_map<uint16_t, MapLegendEntry>& legend_tiles);
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
}