#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "View.h"
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "client/game.h"
#include "client/player.h"

using namespace SDL2pp;

class GameView : public View {
public:
    GameView(Window& window, Renderer& renderer, Game& game, Player& player);
    void init() override;
    void update(Uint32 deltaTime) override;
    void loadMapTiles(Map map);
    void bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, Uint32, bool)> callback);
    SDL_Point getCenterPoint();

private:
    Window& window;
    Renderer& renderer;
    Game& game;
    Player& player;
    std::unordered_map<SDL_EventType, std::function<void(const SDL_Event&, Uint32, bool)>> eventHandlers;
    std::unordered_map<BlockType, std::pair<uint32_t, uint32_t>> tileClipMap;
    Texture mapTiles;
    Texture playerTiles;

    void show();
    BlockType filenameToBlockType(std::string path);
};

#endif // GAMEVIEW_H
