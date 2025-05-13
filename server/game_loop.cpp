#include "game_loop.h"

GameLoop::GameLoop() {}

void GameLoop::run() {
    try {
        std::cout << "GameLoop started" << std::endl;
        while (active) {
            // Game loop logic goes here
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in GameLoop::run: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in GameLoop::run" << std::endl;
    }
    
}

GameLoop::~GameLoop() {
    std::cout << "GameLoop stopped" << std::endl;
}

void GameLoop::stop() {
    active = false;
}