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
    GameView(Window& window, Renderer& renderer, Game& game, Player& player);
    void init() override;
    void update(Uint32 deltaTime) override;
    // void loadMapTiles(Map map);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, Uint32, bool)> callback);
    SDL_Point getCenterPoint();

private:
    Window& window;
    Renderer& renderer;
    Game& game;
    Player& player;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&, Uint32, bool)>> eventHandlers;
    // std::unordered_map<BlockType, std::pair<uint32_t, uint32_t>> tileClipMap;
    Texture mapTiles = Texture(renderer, MAP_TILES_PATH + "aztec.bpm");
    Texture playerTiles = Texture(renderer, PLAYER_TILES_PATH + "ct1.bmp");

    void show();
    std::vector<std::vector<uint32_t>> getPlaceholderMap();
    // BlockType filenameToBlockType(std::string path);
    // std::string mapToTilesFilename(Map map);
};

#endif // GAMEVIEW_H
