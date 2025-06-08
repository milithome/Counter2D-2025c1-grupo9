#include "match.h"
#include "admin.h"

Match::Match(std::string& name, Admin& admin) : name(name), admin(admin), inLobby(true), inGame(false), toMatch(std::make_shared<Queue<Message>>(100)) {}

Match::~Match() {}

void Match::addClient(std::shared_ptr<Client> client) {
    client->receiver->changeQueue(toMatch);
    clients.insert(client);

    toMatch->push({Type::JOIN, 0, "", Action{}, client->channels.name});
}

void Match::run() {
    try {
        lobbyLoop();
        if (inGame) {
            gameLoop();
        }
        admin.removeMatch(name);
    } catch (const std::exception& e) {
        std::cerr << "Exception in Match: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Match." << std::endl;
    }
}

void Match::lobbyLoop() {
    while (inLobby) {
        try {
            Message message = toMatch->pop();
            handleLobbyMessage(message);
            if (!inLobby) {
                return;
            }
            broadcastLobbyState();

            if (clients.size() == maxPlayers) {
                for (const auto& client : clients) {
                    Response response = {
                        Type::LOBBY_READY,
                        0,
                        {},
                        "Lobby is ready"
                    };
                    client->channels.responses->push(response);
                }
            }
        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& e) {
            std::cerr << "Exception in lobby loop: " << e.what() << std::endl;
        }
        
    }
}

void Match::handleLobbyMessage(const Message& message) {
    switch (message.type) {
        case Type::JOIN:
            break;
        case Type::LEAVE:
            handleLeave(message.clientName);
            break;
        case Type::START:
            handleStart();
            break;
        default:
            std::cerr << "Unknown message type: " << static_cast<int>(message.type) << std::endl;
    }
}

void Match::handleLeave(const std::string& clientName) {
    auto it = std::find_if(clients.begin(), clients.end(), [&clientName](const std::shared_ptr<Client>& client) {
        return client->channels.name == clientName;
    });
    
    if (it != clients.end()) {
        auto client = *it;

        Response response = {
            Type::LEAVE,
            0,
            {},
            "Leave successful"
        };
        client->channels.responses->push(response);

        clients.erase(it);

        admin.createMenu(client, true);
    } else {
        std::cerr << "Client " << clientName << " not found in the lobby." << std::endl;
    }

    if (clients.size() == 0) {
        inLobby = false;
    }
}

void Match::handleStart() {
    if (clients.size() < maxPlayers) {
        std::cerr << "Not enough players to start the game." << std::endl;
        return;
    }
    
    inLobby = false;
    inGame = true;

    for (const auto& client : clients) {
        Response response = {
            Type::START,
            0,
            {},
            "Game started"
        };
        client->channels.responses->push(response);
    }
}

void Match::broadcastLobbyState() {
    StateLobby state;
    for (const auto& client : clients) {
        state.players.push_back(client->channels.name);
    }
    
    for (const auto& client : clients) {
        Response response = {
            Type::STATE_LOBBY,
            0,
            state,
            "Lobby state updated for player " + client->channels.name + ". Current players: " + std::to_string(state.players.size())
        };
        client->channels.responses->push(response);
    }
}

void Match::gameLoop() {
    try {
        waitForPlayers();
        Map map("../assets/maps/default.yaml");
        Game game(map.getMapData().game_map);

        setupGame(game);
        broadcastInitialData(map.getMapData());
        startTimeoutThread(game);

        runGameLoop(game);

    } catch (const std::exception& e) {
        std::cerr << "Exception in gameLoop: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in gameLoop." << std::endl;
    }
}

void Match::waitForPlayers() {
    while (clients.size() < maxPlayers) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Match::setupGame(Game& game) {
    for (const auto& client : clients) {
        game.addPlayer(client->channels.name);
    }
}

void Match::startTimeoutThread(Game& game) {
    std::thread timeoutThread([this, &game]() {
        std::this_thread::sleep_for(std::chrono::minutes(1));
        game.stop();
        inGame = false;
        std::cout << "pasaron 10 segundos" << std::endl;
    });
    timeoutThread.detach();
}

void Match::runGameLoop(Game& game) {
    const std::chrono::milliseconds TICK_DURATION(16);
    const uint MAX_EVENTS_PER_CLICK = 32;

    auto lastTime = std::chrono::steady_clock::now();
    inGame = true;

    while (inGame) {
        auto start_time = std::chrono::steady_clock::now();
        processMessages(game, MAX_EVENTS_PER_CLICK);

        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        game.update(deltaTime);
        broadcastGameState(game.getState());
        game.shotQueueClear(); // se tiene q vaciar la cola si los disparos ya fueron procesados

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time
        );
        if (elapsed < TICK_DURATION) {
            std::this_thread::sleep_for(TICK_DURATION - elapsed);
        }

        if (!game.isRunning()) {
            endGame();
        }
    }
}

void Match::processMessages(Game& game, uint maxEvents) {
    uint processedCounter = 0;
    Message message;

    while (processedCounter < maxEvents && toMatch->try_pop(message)) {
        if (message.type == Type::ACTION) {
            game.execute(message.clientName, message.action);
            processedCounter++;
        } else {
            handleGameMessage(message);
        }
    }
}

void Match::endGame() {
    inGame = false;
    for (const auto& client : clients) {
        Response response = {
            Type::FINISH,
            0,
            {},
            "Game finished"
        };
        client->channels.responses->push(response);
        admin.createMenu(client);
    }
}

void Match::broadcastInitialData(const MapData& mapData) {
    std::vector<std::string> playerNames;
    for (const auto& client : clients) {
        playerNames.push_back(client->channels.name);
    }

    InitialData initialData = {
        mapData,
        playerNames
    };

    for (const auto& client : clients) {
        Response response = {
            Type::INITIAL_DATA,
            0,
            initialData,
            "Initial data received successfully"
        };
        client->channels.responses->push(response);
    }
}

void Match::broadcastGameState(const StateGame& state) {
    for (const auto& client : clients) {
        Response response = {
            Type::STATE,
            0,
            state,
            "Game state received successfully"
        };
        client->channels.responses->push(response);
    }
}

void Match::handleGameMessage(const Message& message) {
    switch (message.type) {
        case Type::DISCONNECT:
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                if ((*it)->channels.name == message.clientName) {
                    Response response = {
                        Type::DISCONNECT,
                        0,
                        {},
                        "Disconnect successfull."
                    };
                    (*it)->channels.responses->push(response);
                    clients.erase(it);
                    break;
                }
            }

            admin.removeClient(message.clientName);
            disconnectedClients++;

            if (disconnectedClients == maxPlayers) {
                inGame = false;
            }
            
            break;

        default:
            std::cerr << "Unknown message type: " << static_cast<int>(message.type) << std::endl;
    }
}

void Match::stop() {
    inLobby = false;
    inGame = false;
    toMatch->close();
}