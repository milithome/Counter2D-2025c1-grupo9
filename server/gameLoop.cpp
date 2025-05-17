#include "gameLoop.h"
#include "admin.h"

GameLoop::GameLoop(std::string name, Admin& admin, std::map<std::string, Protocol>&& players)
    : name(name), admin(admin), players(std::move(players)), eventQueue(500) , active(true) {
        for (std::pair<const std::string, Protocol>& pair : this->players) {
            const std::string& name = pair.first;
            Protocol& protocol = pair.second;
        
            auto receiver = std::make_shared<GameReceiver>(protocol, name, eventQueue);
            handlers.emplace(name, std::move(receiver));
        }
    }

void GameLoop::run() {
    try {
        Game game = Game(10,10);

        std::cout << "GameLoop started" << std::endl;
        uint count = 0;
        for (std::pair<const std::string, std::shared_ptr<GameReceiver>>& pair : handlers) {
            pair.second->start();
            game.addPlayer(pair.first,count);
            count++;
        }

        const std::chrono::milliseconds TICK_DURATION(100);
        const uint MAX_EVENTS_PER_CLICK = 20;

        while (active) {
            auto start_time = std::chrono::steady_clock::now();
            
            uint processedCounter = 0;
            ActionEvent event;
            while(processedCounter < MAX_EVENTS_PER_CLICK && eventQueue.try_pop(event)) {
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

            if (game.isRunning()) {
                active = false;
            }
        }

        for (auto& pair : handlers) {
            pair.second->stop();
        }
        for (auto& pair : handlers) {
            pair.second->join();
        }

        admin.endGame(name,std::move(players));

    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::run: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::run" << std::endl;
    }
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