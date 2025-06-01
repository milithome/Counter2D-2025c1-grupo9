#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/game.h"
#include "common/player.h"

#define MAP_TILES_PATH 'assets/gfx/tiles/'
#define PLAYER_TILES_PATH 'assets/gfx/player/'
#define CLIP_SIZE 32
#define BLOCK_SIZE 48
#define BACKGROUND_TEXTURE_SIZE 64

using namespace SDL2pp;

#define SHOT_SPEED 120
#define SHOT_DURATION 10
#define SHOT_THICKNESS 4
#define SHOT_LENGTH 32



struct ShotEffect {
    float x;
    float y;
    float target_x;
    float target_y;
    float angle;
    float time_left;
};


class GameView {
public:
    //GameView(Game& game, const std::string& playerName, SDL_Point window_pos, const std::string& background_path, const std::string& sprite_path, const std::vector<std::vector<uint16_t>>& tiles_map, const std::unordered_map<uint16_t, MapLegendEntry>& legend_tiles);
    GameView(Game& game, const std::string& playerName, SDL_Point window_pos, Map& map);
    Window createWindow(SDL_Point window_pos);
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    SDL_Point getCenterPoint();
    void addShotEffect(Bullet bullet);
    void switchShopVisibility();

private:
    // Mixer mixer = Mixer(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Window window;
    Renderer renderer;
    Game& game;
    std::string playerName;
    Map& map;
    //std::vector<std::vector<uint16_t>> tiles_map;
    //std::unordered_map<uint16_t, MapLegendEntry> legend_tiles;


    Texture mapTiles;
    Texture backgroundTexture;
    Texture playerTiles = Texture(renderer, "../assets/gfx/player/ct1.bmp"); // temporal

    // T skins
    Texture phoenix = Texture(renderer, "../assets/gfx/player/t1.bmp");
    Texture L337Krew = Texture(renderer, "../assets/gfx/player/t2.bmp");
    Texture arcticAvenger = Texture(renderer, "../assets/gfx/player/t3.bmp");
    Texture Guerrilla = Texture(renderer, "../assets/gfx/player/t4.bmp");

    // CT skins
    Texture sealForce = Texture(renderer, "../assets/gfx/player/ct1.bmp");
    Texture germanGSG9 = Texture(renderer, "../assets/gfx/player/ct2.bmp");
    Texture UKSAS = Texture(renderer, "../assets/gfx/player/ct3.bmp");
    Texture frenchGIGN = Texture(renderer, "../assets/gfx/player/ct4.bmp");


    bool shopIsVisible = false;
    std::vector<ShotEffect> shot_effects;

    void showBackground();
    void showMap(float cameraX, float cameraY);
    void showBullets(float cameraX, float cameraY, float deltaTime);
    void showEntities(float cameraX, float cameraY);
    void showInterface();
    void showShop();
};

#endif

