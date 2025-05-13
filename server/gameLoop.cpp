#include "gameLoop.h"

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
        //falta inicilizar la logica del juego
        // game = Game()

        std::cout << "GameLoop started" << std::endl;
        for (std::pair<const std::string, std::shared_ptr<GameReceiver>>& pair : handlers) {
            pair.second->start();
        }

        const std::chrono::milliseconds TICK_DURATION(100);

        while (active) {
            auto start_time = std::chrono::steady_clock::now();
            
            ActionEvent event;
            while(eventQueue.try_pop(event)) {
                switch (event.type)
                {
                case Action::MOVE_LEFT:
                    // game.executeAction(nameplayer, action)
                    std::cout << "The player" << event.playerName << "has moved to the left" << std::endl;
                    break;
                case Action::MOVE_RIGHT:
                    std::cout << "The player" << event.playerName << "has moved to the right" << std::endl;
                    break;
                case Action::MOVE_DOWN:
                    std::cout << "The player" << event.playerName << "has moved down" << std::endl;
                    break;
                case Action::MOVE_UP:
                    std::cout << "The player" << event.playerName << "has moved up" << std::endl;
                    break;
                default:
                    std::cout << "The player" << event.playerName << "has taken an action" << std::endl;
                    break;
                }
            }

            std::vector<Entity> entities = {}; // = game.getState();
            broadcast_game_state(entities);

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            if (elapsed < TICK_DURATION) {
                std::this_thread::sleep_for(TICK_DURATION - elapsed);
            }

            // if game.finalize() {active = false} 
        }

        for (auto& pair : handlers) {
            pair.second->stop();
        }
        for (auto& pair : handlers) {
            pair.second->join();
        }

        admin.endGame(name,players);

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