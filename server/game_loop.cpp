#include "game_loop.h"

GameLoop::GameLoop() {}

void GameLoop::run() {
    std::cout << "GameLoop started" << std::endl;
}

GameLoop::~GameLoop() {
    std::cout << "GameLoop stopped" << std::endl;
}