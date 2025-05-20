#include "gameLoop.h"
#include "admin.h"

GameLoop::GameLoop(std::string name, Admin& admin)
    : name(name), admin(admin), toGame(std::make_shared<Queue<ActionRequest>>(100)), fromPlayers(), active(true), game(10,10) {}

void GameLoop::run() {
    try {
        std::cout << "GameLoop started" << std::endl;

        const std::chrono::milliseconds TICK_DURATION(100);
        const uint MAX_EVENTS_PER_CLICK = 20;

        while(players.size() < 3){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // SIMULO UN MINUTO DE PARTIDA
        std::thread timeoutThread([this]() {
            std::this_thread::sleep_for(std::chrono::minutes(1));
            game.stop();
            active = false;
        });
        timeoutThread.detach();

        while (active) {
            auto start_time = std::chrono::steady_clock::now();
            
            uint processedCounter = 0;
            ActionRequest event;
            while(processedCounter < MAX_EVENTS_PER_CLICK && toGame->try_pop(event)) {
                game.execute(event.playerName, event.action);
                processedCounter++;
            }

            std::vector<Entity> entities = game.getState();
            std::cout << "broadcast" << std::endl;
            broadcast_game_state(entities);

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            if (elapsed < TICK_DURATION) {
                std::this_thread::sleep_for(TICK_DURATION - elapsed);
            }

            //game.stop();

            if (!game.isRunning()) {
                active = false;
            }
        }

        for (auto& [name, queue] : fromPlayers) {
            ActionRequest event = {
                { ActionType::FINISH, std::monostate{} },
                name
            };
            queue->push(event);
        }

        admin.endGame(name);
        players.clear();
        std::cout << "Game finish" << std::endl; 

    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::run: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::run" << std::endl;
    }
}

GameChannels GameLoop::add_player(Protocol& player, const std::string& playerName) {
    players.emplace(playerName, player);
    game.addPlayer(playerName);

    auto fromPlayerQueue = std::make_shared<Queue<ActionRequest>>(100);
    fromPlayers.emplace(playerName,fromPlayerQueue);

    return GameChannels{
        toGame,
        fromPlayerQueue
    };
}

void GameLoop::broadcast_game_state(std::vector<Entity>& entities) {
    for (auto& pair : players) {
        try {
            Response response = {
                Type::STATE,
                0,
                static_cast<uint16_t>(entities.size()),
                entities,
                {},
                {},
                ""
            };
            pair.second.send_response(response);
        } catch (const std::exception& e) {
            std::cerr << "Failed to send to player " << pair.first << ": " << e.what() << std::endl;
        }
    }
}

GameLoop::~GameLoop() {
    std::cout << "GameLoop destructor called." << std::endl;
}

void GameLoop::stop() {
    active = false;
}