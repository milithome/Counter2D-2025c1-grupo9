#include "client.h"

Client::Client(std::string clientName, std::string name_server, int port): clientName(clientName), started(false)
{
    prepare_Queues_and_QT_start(name_server, port);

}

void Client::run() {

    SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	// Menu loop
    timer = std::make_unique<QTimer>(menuController.get());
    timer->start(0);

	QObject::connect(menuController.get(), &MenuController::nuevoEvento, [this](MessageEvent* event) {
        send_queue->try_push(std::shared_ptr<MessageEvent>(event));
    });

	app->exec();

	
	if (!started) {
		game_not_started();
	};
	
    game_started();

	game_ended();
}

void Client::prepare_Queues_and_QT_start(std::string name_server, int port)
{
    Socket clientSocket(name_server.c_str(), std::to_string(port).c_str());
	Protocol protocol(std::move(clientSocket));

    protocol.send_name(clientName);
    
    recv_queue = std::make_unique<Queue<Response>>(100);
    send_queue = std::make_unique<Queue<std::shared_ptr<MessageEvent>>>(100);

    receiver = std::make_unique<RecvLoop>(protocol, recv_queue);
    sender = std::make_unique<SendLoop>(protocol, send_queue);

    if (receiver) {
    receiver->start();
    }
    if (sender) {
        sender->start();
    }
    app = std::make_unique<QApplication>(0, nullptr);
	menuWindow = std::make_unique<QtWindow>(app, "Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT);
	menuController = std::make_unique<MenuController>(menuWindow, protocol);
}

/* void Client::QT_start(Protocol protocol)
{
    app = app(0, nullptr);
	menuWindow = std::make_unique<QtWindow>(app, "Counter Strike 2D", SCREEN_WIDTH, SCREEN_HEIGHT);
	menuController = std::make_unique<MenuController>(menuWindow, protocol);

} */

void Client::connect_Q_object()
{
    QObject::connect(timer, &QTimer::timeout, &menuController, [this] {
        Response msg;
        while (this->recv_queue.try_pop(msg)) {
            recv_from_action(msg);
        }
    });
}

void Client::recv_from_action(Response msg){
    switch (msg.type) {
        case LIST: {
            menu_action_list();
            break;
        }
        case JOIN: {
            menu_action_join();
            break;
        }
        case LEAVE: {
            menu_action_leave();
            break;
        }
        case STATE_LOBBY: {
            menu_action_state_lobby();
            break;
        }
        case START: {
            menu_action_start_game();
            break;
        }
        default: {
            break;
        }
    }
}

void Client::menu_action_list()
{
    LobbyList data = std::get<LobbyList>(msg.data);
    menuController.onPartyListReceived(data.lobbies, msg.message, msg.result);
}

void Client::menu_action_join()
{
    menuController.onJoinPartyResponseReceived(msg.message, msg.result);
}

void Client::menu_action_leave()
{
    menuController.onLeavePartyResponseReceived(msg.message, msg.result);
}

void Client::menu_action_state_lobby()
{
    StateLobby data = std::get<StateLobby>(msg.data);
    players = data.players;
    menuController.onLobbyPlayersReceived(players, msg.message, msg.result);
}

void Client::menu_action_start_game()
{
    started = true;
    w_pos_when_game_started = menuWindow->getPosition();
    menuController.onGameStarted();
}

void Client::game_ended()
{
    if (receiver) {
        receiver->stop();
        receiver->join();
    }
    if (sender) {
        sender->stop();
        sender->join();
    }
}

void Client::game_not_started()
{
    protocol.send_disconnect();
    Response msg = protocol.recv_response();
    std::cout << msg.message << std::endl;
    return 0;
}

void Client::game_started()
{
    game_being_prepared();

	uint32_t lastTime = 0;
	
    while (game.isRunning()) {
		game_running();
	}
}

void Client::game_being_prepared()
{
    Game game(10, 10);
	for (size_t i = 0; i < players.size(); i++) {
		game.addPlayer(players[i]);
	}
	gameView = GameView(game, clientName, SDL_Point{w_pos_when_game_started.x(), w_pos_when_game_started.y()});
	gameController = GameController(gameView, game, clientName);
}

void Client::game_running()
{
    update_sdl_ticks();

    while (!gameController.actionQueueIsEmpty()) {
        send_action_to_queue();
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

void Client::update_sdl_ticks()
{
    uint32_t currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    gameView.update(deltaTime);
    gameController.processEvents();
    gameController.update(deltaTime);
}

void Client::send_action_to_queue()
{
    Action action = gameController.actionQueuePop();
    std::shared_ptr<MessageEvent> event = std::make_shared<ActionEvent>(action);
    send_queue.try_push(event);
}
