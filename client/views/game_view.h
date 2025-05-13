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
    GameView(Window& window, Renderer& renderer, Game& game, uint player_id);
    void init();
    void update(float deltaTime);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, float, bool)> callback);
    void bindLoop(const std::function<void(float)> callback);
    SDL_Point getCenterPoint();

private:
    Window& window;
    Renderer& renderer;
    Game& game;
    uint player_id;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&, float, bool)>> eventHandlers;
    Texture mapTiles = Texture(renderer, "assets/gfx/tiles/aztec.bmp");
    Texture playerTiles = Texture(renderer, "assets/gfx/player/ct1.bmp");
    std::vector<std::function<void(float)>> gameLoopListeners;

    void show();
    std::vector<std::vector<uint32_t>> getPlaceholderMap();
};

#endif // GAMEVIEW_H
