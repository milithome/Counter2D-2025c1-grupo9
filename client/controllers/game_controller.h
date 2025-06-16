#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "client/views/game_sound_handler.h"
#include "client/views/game_view.h"
#include "common/game.h"
#include "common/player.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <queue>
#include <set>

#define FRAMES_UNTIL_SUCCESSFUL_PLANT 240

class GameController {
public:
    GameController(GameView& view, const std::string& player_name, bool pulse_available);
    Action actionQueuePop();
    bool actionQueueIsEmpty();
    void updateGameState(StateGame entities);
    bool processEvents();


private:
    GameView& view;    
    //Game& game;
    StateGame state;
    bool state_available = false;
    PlayerData clientPlayerData;
    GameSoundHandler soundHandler;
    std::string player_name;
    void onKeyPressed(const SDL_Event& event);
    void onKeyReleased(const SDL_Event& event);
    void onWindowEvent(const SDL_Event& event);
    void onQuitPressed();
    void onMouseMovement();
    void onMouseLeftClick(const SDL_Event& event);
    void onMouseLeftClickReleased(const SDL_Event& event);
    std::vector<int> movement_keys_vector = std::vector<int>(2, 0);
    std::queue<Action> action_queue;
    std::vector<Action> actions;

    std::unordered_map<uint32_t, std::pair<float, float>> move_actions;
    std::set<SDL_Keycode> movement_keys = {SDLK_w, SDLK_s, SDLK_d, SDLK_a};
    uint32_t lastMoveIdFromServer = 0;
    uint32_t lastMoveId = 0;

    bool shop_open = false;
    bool planting = false;
    bool defusing = false;

};

#endif