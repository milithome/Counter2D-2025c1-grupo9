#include "gameClient.h"

GameClient::GameClient(Game &game, Map &map, GameView &gameView, std::vector<std::string> players, Queue<Response> &recv_queue, Queue<std::shared_ptr<MessageEvent>> &send_queue, 
                       std::string const clientName, SDL &sdl)
    : sdl(sdl), 
    game(game), 
    gameController(gameView, game, clientName), 
    map(map), gameView(gameView), 
    players(players), 
    recv_queue(recv_queue), 
    send_queue(send_queue)

{
}

void GameClient::run() {
    
	const std::chrono::milliseconds TICK_DURATION(16);

    auto lastTime = std::chrono::steady_clock::now();
	while (game.isRunning()) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

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

		gameController.processEvents();
		gameController.update(deltaTime);
		gameView.update(deltaTime);

		while (!gameController.actionQueueIsEmpty()) {
			Action action = gameController.actionQueuePop();
			std::shared_ptr<MessageEvent> event = std::make_shared<ActionEvent>(action);
			send_queue.try_push(event);
		}

		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - currentTime
        );
        if (elapsed < TICK_DURATION) {
            std::this_thread::sleep_for(TICK_DURATION - elapsed);
        }
	}

}
