#include "gameLoop.h"
#include "admin.h"

GameLoop::GameLoop(std::string name, Admin& admin)
    : name(name), 
    admin(admin), 
    toGame(std::make_shared<Queue<ActionRequest>>(100)), 
    fromPlayers(), 
    active(true), 
    map("../assets/maps/default.yaml") , 
    game(map.getMapData().game_map) {}

void GameLoop::run() {
    try {
        std::cout << "GameLoop started" << std::endl;

        const std::chrono::milliseconds TICK_DURATION(16);
        const uint MAX_EVENTS_PER_CLICK = 32;

        while(players.size() < 3){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        broadcast_initial_data(map.getMapData());

        // SIMULO UN MINUTO DE PARTIDA
        std::thread timeoutThread([this]() {
            std::this_thread::sleep_for(std::chrono::minutes(1));
            game.stop();
            active = false;
        });
        timeoutThread.detach();

        auto lastTime = std::chrono::steady_clock::now();
        while (active) {
            auto start_time = std::chrono::steady_clock::now();

            uint processedCounter = 0;
            ActionRequest event;
            while(processedCounter < MAX_EVENTS_PER_CLICK && toGame->try_pop(event)) {
                game.execute(event.playerName, event.action);
                processedCounter++;
            }

            auto currentTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            game.update(deltaTime);

            StateGame state = game.getState();
            broadcast_game_state(state);

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            if (elapsed < TICK_DURATION) {
                std::this_thread::sleep_for(TICK_DURATION - elapsed);
            }

            if (!game.isRunning()) {
                active = false;
            }
        }

        for (auto& [name, queue] : fromPlayers) {
            ActionRequest event = {
                { ActionType::FINISH, {} },
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

void GameLoop::broadcast_initial_data(const MapData& mapData) {
    for (auto& pair : players) {
        try {
            std::vector<std::string> playerNames;
            for (const auto& player : players) {
                playerNames.push_back(player.first);
            }
            InitialData initialData = {
                mapData,
                playerNames
            };
            Response response = {
                Type::INITIAL_DATA,
                0,
                initialData,
                ""
            };
            pair.second.send_response(response);
        } catch (const std::exception& e) {
            Response response = {
                Type::INITIAL_DATA,
                1,
                InitialData{},
                "Error sending initial data to player " + pair.first + ": " + e.what()
            };
            pair.second.send_response(response);
        }
    }
}

void GameLoop::broadcast_game_state(StateGame& state) {
    for (auto& pair : players) {
        try {
            Response response = {
                Type::STATE,
                0,
                state,
                ""
            };
            pair.second.send_response(response);
        } catch (const std::exception& e) {
            std::cerr << "Failed to send to player " << pair.first << ": " << e.what() << std::endl;
            Response response = {
                Type::STATE,
                1,
                StateGame{},
                "Error sending game state to player " + pair.first
            };
            pair.second.send_response(response);
        }
    }
}

GameLoop::~GameLoop() {}

void GameLoop::stop() {
    active = false;
}