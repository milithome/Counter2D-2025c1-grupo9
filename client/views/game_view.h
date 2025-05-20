#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/game.h"
#include "common/player.h"

#define MAP_TILES_PATH 'assets/gfx/tiles/'
#define PLAYER_TILES_PATH 'assets/gfx/player/'
#define CLIP_SIZE 32
#define BLOCK_SIZE 32

using namespace SDL2pp;

class GameView {
public:
    GameView(Game& game, const std::string& playerName);
    Window createWindow();
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&)> callback);
    void bindLoop(const std::function<void(float)> callback);
    SDL_Point getCenterPoint();

private:
    Window window;
    Renderer renderer;
    Game& game;
    std::string playerName;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&)>> eventHandlers;
    Texture mapTiles = Texture(renderer, "../assets/gfx/tiles/aztec.bmp");
    Texture playerTiles = Texture(renderer, "../assets/gfx/player/ct1.bmp");
    std::vector<std::function<void(float)>> gameLoopListeners;

    void show();
    std::vector<std::vector<uint32_t>> getPlaceholderMap();
};

#endif
