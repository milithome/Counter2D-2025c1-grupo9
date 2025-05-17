#include "gameReceiver.h"

GameReceiver::GameReceiver(Protocol& player, const std::string& playerName, Queue<ActionEvent>& eventQueue)
    : active(true), player(player), playerName(playerName), eventQueue(eventQueue) {}

void GameReceiver::run() {
    try {
        while (active) {
            Message msg = player.recv_message();
            ActionEvent event;
            switch (msg.type)
            {
            case Type::ACTION:
                event.action = msg.action;
                event.playerName = playerName;
                eventQueue.push(std::move(event));
                break;
            default:
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in GameReceiver::run for " << playerName << ": " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameReceiver::run for " << playerName << std::endl;
    }
}

GameReceiver::~GameReceiver() {}

void GameReceiver::stop() {
    active = false;
}