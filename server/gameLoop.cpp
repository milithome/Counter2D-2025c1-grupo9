#include "gameLoop.h"
#include "admin.h"

GameLoop::GameLoop(std::string name, Admin& admin)
    : name(name), admin(admin), toGame(100), fromPlayers(), active(true), game(10,10) {}

void GameLoop::run() {
    try {
        std::cout << "GameLoop started" << std::endl;

        const std::chrono::milliseconds TICK_DURATION(100);
        const uint MAX_EVENTS_PER_CLICK = 20;

        while(players.size() < 4){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (active) {
            auto start_time = std::chrono::steady_clock::now();
            
            uint processedCounter = 0;
            ActionEvent event;
            while(processedCounter < MAX_EVENTS_PER_CLICK && toGame.try_pop(event)) {
                game.execute(event.playerName, event.action);
                processedCounter++;
            }

            std::vector<Entity> entities = game.getState();
            broadcast_game_state(entities);

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            if (elapsed < TICK_DURATION) {
                std::this_thread::sleep_for(TICK_DURATION - elapsed);
            }

            game.stop(); // BORRAR EN ALGUN MOMENTO

            if (!game.isRunning()) {
                active = false;
            }
        }

        admin.endGame(name);
        for (auto& [name, queue] : fromPlayers) {
            ActionEvent event = {
                { ActionType::FINISH, std::monostate{} },
                name
            };
            queue.push(event);
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::run: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::run" << std::endl;
    }
}

GameChannels GameLoop::add_player(Protocol& player, const std::string& name) {
    auto toGame = std::make_unique<Queue<LobbyEvent>>();
    auto fromGame = std::make_unique<Queue<LobbyEvent>>();

    players.emplace(name, player);
    game.addPlayer(name,0);

    return GameChannels{
        std::make_unique<Queue<ActionEvent>>(100),
        std::make_unique<Queue<ActionEvent>>(100)
    };
}

void GameLoop::broadcast_game_state(std::vector<Entity>& entities) {
    for (auto& pair : players) {
        pair.second.send_state(entities);
    }
}

GameLoop::~GameLoop() {
    std::cout << "GameLoop stopped" << std::endl;
}

void GameLoop::stop() {
    active = false;
}