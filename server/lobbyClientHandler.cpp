#include "lobbyClientHandler.h"

LobbyClientHandler::LobbyClientHandler(Protocol& player, const std::string& playerName, Queue<LobbyEvent>& eventQueue)
    : player(player), playerName(playerName), eventQueue(eventQueue), active(true) {}

void LobbyClientHandler::run() {
    try {
        while (active) {
            Message msg = player.recv_message();
            if (msg.type == Type::LEAVE) {
                std::cout << "Player " << playerName << " sent LEAVE." << std::endl;
                eventQueue.push({LobbyEventType::LEAVE, playerName});
                active = false;
                continue; 
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LobbyClientHandler: " << e.what() << std::endl;
        eventQueue.push({LobbyEventType::LEAVE, playerName});
    } catch (...) {
        std::cerr << "Unknown error in LobbyClientHandler." << std::endl;
        eventQueue.push({LobbyEventType::LEAVE, playerName});
    }
    active = false;
}

void LobbyClientHandler::stop() {
    active = false;
}

LobbyClientHandler::~LobbyClientHandler() {
    std::cout << "LobbyClientHandler destructor called." << std::endl;
}