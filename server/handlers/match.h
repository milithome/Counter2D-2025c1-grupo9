#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <unordered_set>
#include "common/utilities/thread.h"
#include "server/admin.h"
#include "common/utilities/queue.h"
#include "common/utilities/config.h"
#include "common/structures.h"
#include "server/gameLogic/game.h"

class Match : public Thread {
public:
    Match(std::string& name, Admin& admin);

    ~Match() override;

    void addClient(std::shared_ptr<Client> client);
    bool isInLobby() const { return inLobby; }
    bool isInGame() const { return inGame; }

    void run() override;
    void stop() override;
private:
    std::string name;
    Admin& admin;
    bool inLobby;
    bool inGame;
    size_t minPlayers;
    size_t maxPlayers;
    std::string mapName = "big";
    std::vector<PlayerInfo> playersInfo;
    std::shared_ptr<Queue<Message>> toMatch;
    GameRules gameRules = load_game_rules("/etc/taller/config_server.yaml");
    std::unordered_set<std::shared_ptr<Client>> clients;
    size_t disconnectedClients;
    
    void lobbyLoop();
    void handleLobbyMessage(const Message& message);
    void handleLeave(const std::string& clientName);
    void handleJoin(const std::string& clientName);
    void handleAction(const Action& action, const std::string& clientName);
    void handleStart();
    void handleDisconnect(const std::string& clientName);
    void broadcastLobbyState();
    void updateLobbyReadyStatus(bool& lobbyReadySent);
    void sendLobbyReadyToAll();
    void sendNotLobbyReadyToAll();
    void gameLoop();
    void waitForPlayers();
    void setupGame(Game& game);
    void runGameLoop(Game& game);
    void processMessages(Game& game, uint maxEvents);
    void endGame();
    void broadcastInitialData(const MapData& mapData, GameRules& gameRules);
    void broadcastGameState(const StateGame& state);
    void handleGameMessage(const Message& message);
};

#endif