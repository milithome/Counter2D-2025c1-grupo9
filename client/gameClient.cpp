#include "gameClient.h"

GameClient::GameClient(
	Map &map,
	GameView &gameView,
	std::vector<std::string> players,
	Queue<Response> &recv_queue,
	Queue<std::shared_ptr<MessageEvent>> &send_queue, 
    std::string const clientName,
	bool audio_available
) :
    gameController(gameView, clientName, audio_available), 
    map(map), gameView(gameView), 
    players(players), 
    recv_queue(recv_queue), 
    send_queue(send_queue) {}

bool GameClient::run() {
    
	const std::chrono::milliseconds TICK_DURATION(16);

    auto lastTime = std::chrono::steady_clock::now();
	bool state_available = false;
	while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

		Response msg;
        while (recv_queue.try_pop(msg)) {
			std::cout << "poppeando mensaje gameClient" << std::endl;
			switch (msg.type) {
				case STATE: {
					StateGame data = std::get<StateGame>(msg.data);
					gameView.updateState(data);
					gameController.updateGameState(data);
					state_available = true;
					break;
				}
				case FINISH: {
					return false;
				}
				default: {
					break;
				}
			}
		}
		if (!state_available) continue;
		bool quit = gameController.processEvents();
		if (quit) {
			return true;
		}
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
	return false;
}
