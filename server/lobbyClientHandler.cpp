#include "lobbyClientHandler.h"

LobbyClientHandler::LobbyClientHandler(Protocol& player, const std::string& playerName, Queue<LobbyEvent>& eventQueue)
    : player(player), playerName(playerName), eventQueue(eventQueue), active(true) {}

void LobbyClientHandler::run() {
    try {
        while (active) {
            if (player.has_data(100)) {
                Message msg = player.recv_message();
                if (msg.type == Type::LEAVE) {
                    active = false;
                    eventQueue.push({LobbyEventType::LEAVE, playerName});
                    continue;
                } 
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LobbyClientHandler: " << e.what() << std::endl;
        eventQueue.push({LobbyEventType::LEAVE, playerName});
    } catch (...) {
        std::cerr << "Unknown error in LobbyClientHandler." << std::endl;
        eventQueue.push({LobbyEventType::LEAVE, playerName});
    }
}

void LobbyClientHandler::stop() {
    active = false;
}

LobbyClientHandler::~LobbyClientHandler() {
    std::cout << "LobbyClientHandler destructor called." << std::endl;
}