#include "game_view.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
namespace fs = std::filesystem;


GameView::GameView(Game& game, const std::string& playerName, SDL_Point window_pos)
    : window(createWindow(window_pos)), renderer(createRenderer(window)), game(game), playerName(playerName) {
        renderer.SetDrawColor(0, 0, 0, 255);
        // loadMapTiles(game.getMapEnum());
        // loadPlayerTiles(player.getType());
}



Window GameView::createWindow(SDL_Point window_pos) {
	return Window("Counter Strike 2D",
			window_pos.x, window_pos.y,
			640, 480,
			SDL_WINDOW_RESIZABLE);
}

Renderer GameView::createRenderer(Window& window) {
    return Renderer(window, -1, SDL_RENDERER_ACCELERATED);
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


void GameView::update(float deltaTime) {
    renderer.Clear();


    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_EventType etype = static_cast<SDL_EventType>(e.type);
        if (eventHandlers.contains(etype)) {
            std::function<void(const SDL_Event&)> handler = eventHandlers[etype];
            handler(e);
        }
    }
    for (size_t i = 0; i < gameLoopListeners.size(); i++) {
        gameLoopListeners[i](deltaTime);
    }

    // graficar
    show(deltaTime);

    renderer.Present();

}

void GameView::show(float deltaTime) {
    auto map = getPlaceholderMap(); // temporal, hasta que definamos bien el mapa
    auto gameState = game.getState();
    float clientPlayerX;
    float clientPlayerY;
    for (size_t i = 0; i < gameState.size(); i++) {
        if (gameState[i].name == playerName) {
            clientPlayerX = gameState[i].x;
            clientPlayerY = gameState[i].y;
            break;
        }
    }

    SDL_Point center = getCenterPoint();
    float cameraX = center.x - clientPlayerX * BLOCK_SIZE - BLOCK_SIZE/2;
    float cameraY = center.y - clientPlayerY * BLOCK_SIZE - BLOCK_SIZE/2;

    for (size_t i = 0; i < map.size(); i++) {
        for (size_t j = 0; j < map[i].size(); j++) {
            std::pair<uint32_t, uint32_t> clip = map[i][j] == 0 ? std::pair(0, 0) : std::pair(32, 0); // temporal, hasta que definamos bien el mapa
            Rect src(clip.first, clip.second, CLIP_SIZE, CLIP_SIZE);
            Rect dst(cameraX + j * BLOCK_SIZE, cameraY + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            renderer.Copy(mapTiles, src, dst);
        }
    }
    Rect src(0, 0, CLIP_SIZE, CLIP_SIZE); // temporal, hasta que definamos bien como se deberian ver los jugadores
    for (size_t i = 0; i < gameState.size(); i++) {
        float playerX = gameState[i].x;
        float playerY = gameState[i].y;
        Rect dst(cameraX + playerX * BLOCK_SIZE, cameraY + playerY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        renderer.Copy(playerTiles, src, dst, gameState[i].rotation + 90.0f, Point(CLIP_SIZE / 2, CLIP_SIZE / 2), SDL_FLIP_NONE);
    }

    while (!game.shotEventQueueIsEmpty()) {
        ShotEvent shot = game.shotEventQueuePop();

        shot_effects.push_back(ShotEffect{shot.origin_x, shot.origin_y, shot.target_x, shot.target_y, shot.angle, SHOT_DURATION});

        Chunk sound("../assets/sfx/weapons/usp_silenced.wav");
        mixer.PlayChannel(-1, sound, 0);
    }

    for (auto it = shot_effects.begin(); it != shot_effects.end();) {
        ShotEffect& shot = *it;

        if (shot.time_left <= 0) {
            it = shot_effects.erase(it);
            continue;
        } else {
            ++it;
        }
        float radians = shot.angle * M_PI / 180.0 ;
        float dx = std::cos(radians);
        float dy = std::sin(radians);

        float previous_distance_to_target = BLOCK_SIZE * std::sqrt((shot.x - shot.target_x) * (shot.x - shot.target_x) + (shot.y - shot.target_y) * (shot.y - shot.target_y));

        shot.x += dx * SHOT_SPEED * deltaTime;
        shot.y += dy * SHOT_SPEED * deltaTime;

        shot.time_left -= deltaTime; 


        float distance_to_target = BLOCK_SIZE * std::sqrt((shot.x - shot.target_x) * (shot.x - shot.target_x) + (shot.y - shot.target_y) * (shot.y - shot.target_y));

        if (distance_to_target > previous_distance_to_target) {
            shot.time_left = 0;
            continue;
        }
        int length = static_cast<int>(std::min(distance_to_target, SHOT_LENGTH)) + 1;
        Surface surface(
            0, length, SHOT_THICKNESS, 32,
            0x00FF0000, // Rmask
            0x0000FF00, // Gmask
            0x000000FF, // Bmask
            0xFF000000  // Amask
        );
        surface.FillRect(SDL2pp::NullOpt,
                        SDL_MapRGBA(surface.Get()->format, 255, 240, 0, 255));
        
        
        Texture texture(renderer, surface);
        
        Rect dst(
            cameraX + shot.x * BLOCK_SIZE, 
            cameraY + shot.y * BLOCK_SIZE, 
            SHOT_THICKNESS, 
            length);
        
        renderer.Copy(
            texture, 
            NullOpt, 
            dst, 
            shot.angle - 90.0f, 
            Point(0, SHOT_THICKNESS / 2), 
            SDL_FLIP_NONE);    
    }
}

void GameView::bind(SDL_EventType eventType, const std::function<void(const SDL_Event&)> callback) {
    eventHandlers[eventType] = callback;
}

void GameView::bindLoop(const std::function<void(float)> callback) {
    gameLoopListeners.push_back(callback);
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
