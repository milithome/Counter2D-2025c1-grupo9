#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "client/views/game_view.h"
#include "common/game.h"
#include "common/player.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>

#define FRAMES_UNTIL_SUCCESSFUL_PLANT 240

class GameController {
public:
    GameController(GameView& view, Game& game, Player& player);


private:
    GameView& view;    
    Game& game;        
    Player& player;    
    void listen();
    void onKeyPressed(const SDL_Event& event, Uint32 deltaTime, bool is_last_event_this_frame);
    void onQuitPressed(const SDL_Event& event);
    void onMouseMovement(const SDL_Event& event, Uint32 deltaTime);
    void onMouseLeftClick(const SDL_Event& event, Uint32 deltaTime, bool is_last_event_this_frame);
    std::vector<float> movement_keys_vector =std::vector<float>(2, 0.0f);
    SDL_Point mouse_position;
    bool pressed_plant_button_this_frame = false;
    bool pressed_shoot_button_this_frame = false;
    uint32_t frames_elapsed_since_planting_started = 0;
};

#endif