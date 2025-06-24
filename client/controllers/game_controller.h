#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "client/views/game_sound_handler.h"
#include "client/views/game_view.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <queue>
#include <set>


class GameController {
public:
    GameController(GameView& view, const std::string& player_name, Times times, bool pulse_available);
    Action actionQueuePop();
    bool actionQueueIsEmpty();
    void updateGameState(StateGame entities);
    bool processEvents();


private:
    GameView& view;    
    StateGame state;
    PlayerData clientPlayerData;
    GameSoundHandler soundHandler;
    std::string player_name;
    Times times;

    void onKeyPressed(const SDL_Event& event);
    void onKeyReleased(const SDL_Event& event);
    void onMouseMovement();
    void onMouseLeftClick(const SDL_Event& event);
    void onMouseLeftClickOnShop(const SDL_Event& event);
    void onMouseLeftClickReleased(const SDL_Event& event);

    std::vector<int> movement_keys_vector = std::vector<int>(2, 0);
    std::queue<Action> action_queue;
    std::vector<Action> actions;

    std::unordered_map<uint32_t, std::pair<float, float>> move_actions;
    std::set<SDL_Keycode> movement_keys = {SDLK_w, SDLK_s, SDLK_d, SDLK_a};

    bool state_available = false;
    bool shop_open = false;
    bool planting = false;
    bool defusing = false;

};

#endif