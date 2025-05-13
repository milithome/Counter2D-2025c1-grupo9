#include "game_view.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
namespace fs = std::filesystem;


GameView::GameView(Window& window, Renderer& renderer, Game& game, Player& player)
    : window(window), renderer(renderer), game(game), player(player) {

        // loadMapTiles(game.getMapEnum());
        // loadPlayerTiles(player.getType());
}

// void GameView::loadMapTiles(Map map) {

//     std::string path = mapToTilesFilename(map);
//     mapTiles = Texture(renderer, path);
//     createTileClipMap(mapTiles, map);
// }


// std::string GameView::mapToTilesFilename(Map map) {
//     return std::string(MAP_TILES_PATH) + "de_aztec.bpm";
// }

// void createTileClipMap(Texture mapTiles, Map map) {

// }

// BlockType GameView::filenameToBlockType(std::string path) {

// };

// void GameView::loadPlayerTiles(PlayerType playerType) {
//     std::string path = PlayerTypeToTilesFilename(playerType);
//     playerTiles = Texture(renderer, path);
// }


void GameView::update(Uint32 deltaTime) {
    renderer.Clear();

    std::vector<SDL_Event> eventQueue;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        eventQueue.push_back(e);
    }
    for (size_t i = 0; i < eventQueue.size(); ++i) {
        const SDL_Event& e = eventQueue[i];
        SDL_EventType etype = static_cast<SDL_EventType>(e.type);
        std::function<void(const SDL_Event&, Uint32, bool)> handler = eventHandlers[etype];
        handler(e, deltaTime, i == eventQueue.size() - 1);
    }

    // graficar
    show();



    renderer.Present();

}

void GameView::show() {
    //std::vector<std::vector<BlockType>> map = game.getMap();
    auto map = getPlaceholderMap();

    for (size_t i = 0; i < map.size(); i++) {
        for (size_t j = 0; j < map[i].size(); j++) {
            //std::pair<uint32_t, uint32_t> clip = tileClipMap[map[i][j]];
            std::pair<uint32_t, uint32_t> clip = map[i][j] == 0 ? std::pair(0, 0) : std::pair(32, 0);
            Rect src(clip.first, clip.second, CLIP_SIZE, CLIP_SIZE);
            Rect dst((player.getY() - j) * BLOCK_SIZE, (player.getX() - i) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            renderer.Copy(mapTiles, src, dst);
        }
    }
    Rect src(0, 0, 32, 32);
    SDL_Point center = getCenterPoint();
    Rect dst(center.x, center.y, BLOCK_SIZE, BLOCK_SIZE);
    renderer.Copy(playerTiles, src, dst);
}

void GameView::bind(SDL_EventType eventType, const std::function<void(const SDL_Event&, Uint32, bool)> callback) {
    eventHandlers[eventType] = callback;
}


SDL_Point GameView::getCenterPoint() {
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    SDL_Point center = { width / 2, height / 2 };
    return center;
}


std::vector<std::vector<uint32_t>> GameView::getPlaceholderMap() {
    const int width = 10;
    const int height = 10;

    std::vector<std::vector<uint32_t>> map(height, std::vector<uint32_t>(width, 0));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                map[y][x] = 1;
            }
        }
    }

    return map;
}
