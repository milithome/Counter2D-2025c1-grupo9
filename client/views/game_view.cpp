#include "game_view.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
namespace fs = std::filesystem;


// GameView::GameView(Game& game, const std::string& playerName, SDL_Point window_pos, const std::string& background_path, const std::string& sprite_path, const std::vector<std::vector<uint16_t>>& tiles_map, const std::unordered_map<uint16_t, MapLegendEntry>& legend_tiles)
//     : window(createWindow(window_pos)), renderer(createRenderer(window)), game(game), playerName(playerName), mapTiles(Texture(renderer, sprite_path)), backgroundTexture(renderer, background_path) {
//         renderer.SetDrawColor(0, 0, 0, 255);
//         // loadMapTiles(game.getMapEnum());
//         // loadPlayerTiles(player.getType());
// }

GameView::GameView(Game& game, const std::string& playerName, SDL_Point window_pos, Map& map)
    : window(createWindow(window_pos)), renderer(createRenderer(window)), game(game), playerName(playerName), map(map), mapTiles(Texture(renderer, map.get_sprite_path())), backgroundTexture(renderer, map.get_background_path()) {
        renderer.SetDrawColor(0, 0, 0, 255);
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


void GameView::update(float deltaTime) {
    renderer.Clear();


    // graficar
    SDL_Point center = getCenterPoint();
    float cameraX = center.x - game.getX(playerName) * BLOCK_SIZE - BLOCK_SIZE/2;
    float cameraY = center.y - game.getY(playerName) * BLOCK_SIZE - BLOCK_SIZE/2;


    showMap(cameraX, cameraY);
    showBullets(cameraX, cameraY, deltaTime);
    showEntities(cameraX, cameraY);
    showInterface();
    if (shopIsVisible) {
        showShop();
    }

    // TODO: sonido
    // Chunk sound("../assets/sfx/weapons/usp_silenced.wav");
    // mixer.PlayChannel(-1, sound, 0);

    renderer.Present();

}

void GameView::showMap(float cameraX, float cameraY) {
    auto tiles_map = map.get_tiles_map();
    for (size_t i = 0; i < tiles_map.size(); i++) {
        for (size_t j = 0; j < tiles_map[i].size(); j++) {
            uint16_t tile = tiles_map[i][j];
            MapLegendEntry clip = map.get_tiles_legend(tile);
            Rect src(clip.x, clip.y, CLIP_SIZE, CLIP_SIZE);
            Rect dst(cameraX + j * BLOCK_SIZE, cameraY + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            renderer.Copy(mapTiles, src, dst);
        }
    }
}

void GameView::showBullets(float cameraX, float cameraY, float deltaTime) {

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

        int32_t previous_distance_to_target = BLOCK_SIZE * std::sqrt((shot.x - shot.target_x) * (shot.x - shot.target_x) + (shot.y - shot.target_y) * (shot.y - shot.target_y));

        shot.x += dx * SHOT_SPEED * deltaTime;
        shot.y += dy * SHOT_SPEED * deltaTime;

        shot.time_left -= deltaTime; 


        int32_t distance_to_target = BLOCK_SIZE * std::sqrt((shot.x - shot.target_x) * (shot.x - shot.target_x) + (shot.y - shot.target_y) * (shot.y - shot.target_y));

        if (distance_to_target > previous_distance_to_target) {
            shot.time_left = 0;
            continue;
        }
        int32_t length = std::min(distance_to_target, SHOT_LENGTH) + 1;
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

void GameView::showEntities(float cameraX, float cameraY) {
    std::vector<Entity> gameState = game.getState().entities;
    Rect src(0, 0, CLIP_SIZE, CLIP_SIZE); // temporal, hasta que definamos bien como se deberian ver los jugadores
    for (size_t i = 0; i < gameState.size(); i++) {
        switch (gameState[i].type) {
            case PLAYER: {
                PlayerData data = std::get<PlayerData>(gameState[i].data);
                float playerX = gameState[i].x;
                float playerY = gameState[i].y;
                Rect dst(cameraX + playerX * BLOCK_SIZE, cameraY + playerY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                renderer.Copy(playerTiles, src, dst, data.rotation + 90.0f, Point(CLIP_SIZE / 2, CLIP_SIZE / 2), SDL_FLIP_NONE);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void GameView::showInterface() {
}

void GameView::showShop() {
}


SDL_Point GameView::getCenterPoint() {
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    SDL_Point center = { width / 2, height / 2 };
    return center;
}



void GameView::addShotEffect(Bullet bullet) {
    shot_effects.push_back(ShotEffect{bullet.origin_x, bullet.origin_y, bullet.target_x, bullet.target_y, bullet.angle, SHOT_DURATION});
}


void GameView::switchShopVisibility() {
    shopIsVisible = !shopIsVisible;
}