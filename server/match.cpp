#include "match.h"
#include "admin.h"

Match::Match(std::string& name, Admin& admin) : name(name), admin(admin), inLobby(true), inGame(false), toMatch(std::make_shared<Queue<Message>>(100)) {
    minPlayers = gameRules.min_players_per_team*2;
    maxPlayers = gameRules.max_players_per_team*2;
}

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
    bool lobbyReadySent = false;
    try {
        while (inLobby) {
            Message message = toMatch->pop();
            handleLobbyMessage(message);

            if (!inLobby) return;

            broadcastLobbyState();
            updateLobbyReadyStatus(lobbyReadySent);
        }
    } catch (const ClosedQueue&) {
        std::cout << "[" << name << "] Closing Lobby." << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Lobby." << std::endl;
    }
}

void Match::handleLobbyMessage(const Message& message) {
    switch (message.type) {
        case Type::JOIN:
            handleJoin(message.clientName);
            break;
        case Type::LEAVE:
            handleLeave(message.clientName);
            break;
        case Type::ACTION:
            handleAction(message.action, message.clientName);
            break;
        case Type::START:
            handleStart();
            break;
        case Type::DISCONNECT:
            handleDisconnect(message.clientName);
            break;
        default:
            std::cerr << "Unknown message type: " << static_cast<int>(message.type) << std::endl;
    }
}

void Match::handleJoin(const std::string& clientName) {
    playersInfo.push_back(PlayerInfo{
        clientName,
        tSkin::PHOENIX,
        ctSkin::SEAL_FORCE
    });

}

void Match::handleLeave(const std::string& clientName) {
    auto it = std::find_if(
        clients.begin(), 
        clients.end(),
        [&clientName](const std::shared_ptr<Client>& client) {
            return client->channels.name == clientName;
        }
    );

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

        playersInfo.erase(
            std::remove_if(
                playersInfo.begin(), 
                playersInfo.end(),
                [&clientName](const PlayerInfo& info) {
                    return info.name == clientName;
                }
            ),
            playersInfo.end()
        );

        admin.createMenu(client, true);
    } else {
        std::cerr << "Client " << clientName << " not found in the lobby." << std::endl;
    }

    if (clients.size() == 0) {
        inLobby = false;
    }
}

void Match::handleAction(const Action& action, const std::string& clientName) {
    auto it = std::find_if(
        playersInfo.begin(), 
        playersInfo.end(),
        [&clientName](const PlayerInfo& info) {
            return info.name == clientName;
        }
    );

    if (it == playersInfo.end()) {
        std::cerr << "PlayerInfo not found for client: " << clientName << std::endl;
        return;
    }
    
    switch (action.type)
    {
    case ActionType::SELECT_T_SKIN: {
        const SelectTSkin& tSkinData = std::get<SelectTSkin>(action.data);
        it->terroristSkin = tSkinData.terroristSkin;
        break;
    }
    case ActionType::SELECT_CT_SKIN: {
        const SelectCTSkin& ctSkinData = std::get<SelectCTSkin>(action.data);
        it->counterTerroristSkin = ctSkinData.counterTerroristSkin;
        break;
    }
    case ActionType::SELECT_MAP: {
        const SelectMap& mapData = std::get<SelectMap>(action.data);
        // haria falta una verificacion de que sea un mapa que exista -> ver mejor cuando haya un editor
        mapName = mapData.name;
        break;
    }
    default:
        break;
    }
}

void Match::handleStart() {
    if (clients.size() < minPlayers || clients.size() > maxPlayers) {
        std::cerr << "Incorrect number of players to start the game." << std::endl;
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

void Match::handleDisconnect(const std::string& clientName) {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->channels.name == clientName) {
            clients.erase(it);
            break;
        }
    }
    admin.removeClient(clientName);
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

void Match::updateLobbyReadyStatus(bool& lobbyReadySent) {
    if (clients.size() >= minPlayers && clients.size() <= maxPlayers) {
        sendLobbyReadyToAll();
        lobbyReadySent = true;
    } else if (lobbyReadySent) {
        sendNotLobbyReadyToAll();
        lobbyReadySent = false;
    }
}

void Match::sendLobbyReadyToAll() {
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

void Match::sendNotLobbyReadyToAll() {
    for (const auto& client : clients) {
        Response response = {
            Type::NOT_LOBBY_READY,
            0,
            {},
            "Lobby isn't ready"
        };
        client->channels.responses->push(response);
    }
}

void Match::gameLoop() {
    try {
        Map map("/var/taller/maps/" + mapName + ".yaml");
        Game game(map.getMapData().game_map, gameRules);

        setupGame(game);
        broadcastInitialData(map.getMapData(), gameRules);
        runGameLoop(game);

    } catch (const ClosedQueue&) {
        std::cout << "[" << name << "]" << " Closing Game." << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in gameLoop." << std::endl;
    }
}

void Match::setupGame(Game& game) {
    for (const auto& client : clients) {
        game.addPlayer(client->channels.name);
    }
}

void Match::runGameLoop(Game& game) {
    auto& ServerConfig = admin.getServerConfig();
    const std::chrono::milliseconds TICK_DURATION(1000/ServerConfig.tick_rate);
    const uint MAX_EVENTS_PER_CLICK = ServerConfig.max_events_per_tick;

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
        game.shotQueueClear();

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
        admin.createMenu(client, true);
    }
}

void Match::broadcastInitialData(const MapData& mapData, GameRules& gameRules) {
    std::vector<WeaponInfo> weaponsInfo;
    std::vector<WeaponName> shopWeapons;

    for (const auto& [name, weapon] : gameRules.weapons) {
        weaponsInfo.push_back({
            name,
            weapon.price,
            weapon.maxAmmo
        });

        if (weapon.purchasable) {
            shopWeapons.push_back(name);
        }
        
    }

    Shop shop = {
        shopWeapons,
        gameRules.ammo_price,
        gameRules.ammo_price
    };

    Times times = {
        gameRules.purchase_duration,
        gameRules.bomb_duration,
        gameRules.time_to_plant,
        gameRules.time_until_new_round
    };

    InitialData initialData = {
        mapData,
        playersInfo,
        weaponsInfo,
        shop,
        times
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
            handleDisconnect(message.clientName);
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