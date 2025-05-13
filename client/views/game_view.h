#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "view.h"
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/game.h"
#include "common/player.h"

#define MAP_TILES_PATH 'assets/gfx/tiles/'
#define PLAYER_TILES_PATH 'assets/gfx/player/'
#define CLIP_SIZE 32
#define BLOCK_SIZE 32

using namespace SDL2pp;

class GameView : public View {
public:
<<<<<<< Updated upstream
    GameView(Window& window, Renderer& renderer, Game& game, Player& player);
    void init() override;
    void update(Uint32 deltaTime) override;
    // void loadMapTiles(Map map);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, Uint32, bool)> callback);
=======
    GameView(Window& window, Renderer& renderer, Game& game, uint player_id);
    void init();
    void update(float deltaTime);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, float, bool)> callback);
    void bindLoop(const std::function<void(float)> callback);
>>>>>>> Stashed changes
    SDL_Point getCenterPoint();

private:
    Window& window;
    Renderer& renderer;
    Game& game;
<<<<<<< Updated upstream
    Player& player;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&, Uint32, bool)>> eventHandlers;
    // std::unordered_map<BlockType, std::pair<uint32_t, uint32_t>> tileClipMap;
    Texture mapTiles = Texture(renderer, MAP_TILES_PATH + "aztec.bpm");
    Texture playerTiles = Texture(renderer, PLAYER_TILES_PATH + "ct1.bmp");
=======
    uint player_id;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&, float, bool)>> eventHandlers;
    Texture mapTiles = Texture(renderer, "assets/gfx/tiles/aztec.bmp");
    Texture playerTiles = Texture(renderer, "assets/gfx/player/ct1.bmp");
    std::vector<std::function<void(float)>> gameLoopListeners;
>>>>>>> Stashed changes

    void show();
    std::vector<std::vector<uint32_t>> getPlaceholderMap();
};

#endif // GAMEVIEW_H
