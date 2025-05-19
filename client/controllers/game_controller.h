#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H


#include "client/views/game_view.h"
#include "common/game.h"
#include "common/player.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <queue>

#define FRAMES_UNTIL_SUCCESSFUL_PLANT 240

class GameController {
public:
    GameController(GameView& view, Game& game, const std::string& player_name);
    Action actionQueuePop();
    bool actionQueueIsEmpty();
    std::vector<Action> getActions();

private:
    GameView& view;    
    Game& game;        
    std::string player_name;
    void listen();
    void onKeyPressed(const SDL_Event& event);
    void onKeyReleased(const SDL_Event& event);
    void onQuitPressed();
    void onMouseMovement();
    void onMouseLeftClick(const SDL_Event& event);
    void update(float deltaTime);
    std::vector<float> movement_keys_vector = std::vector<float>(2, 0.0f);
    std::queue<Action> action_queue;
    std::vector<Action> actions;
};

#endif