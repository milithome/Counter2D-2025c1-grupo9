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
#define BACKGROUND_TEXTURE_SIZE 64

using namespace SDL2pp;

#define SHOT_SPEED 120
#define SHOT_DURATION 10
#define SHOT_THICKNESS 2
#define SHOT_LENGTH 8


#define FONT_SIZE 32


struct ShotEffect {
    float x;
    float y;
    float target_x;
    float target_y;
    float angle;
    float time_left;
};



#define PARTICLE_SIZE 2
#define PARTICLE_ACCELERATION -60


#define BLOOD_EFFECT_PARTICLES 16
#define BLOOD_PARTICLE_DISPERSION 10
#define BLOOD_PARTICLE_MAX_DURATION 0.5
#define BLOOD_PARTICLE_MIN_DURATION 0.2
#define BLOOD_PARTICLE_MIN_SPEED 10
#define BLOOD_PARTICLE_MAX_SPEED 20

#define SPARK_EFFECT_PARTICLES 8
#define SPARK_PARTICLE_MAX_DURATION 0.2
#define SPARK_PARTICLE_MIN_DURATION 0.1
#define SPARK_PARTICLE_MIN_SPEED 3
#define SPARK_PARTICLE_MAX_SPEED 6


struct Particle {
    float x, y;
    float speed;
    float angle;
    float time_left;
};

struct HitEffect {
    std::vector<Particle> particles;
};


// d inventory, dropped
// m shop
// _ ingame

class GameView {
public:
    GameView(Game& game, const std::string& playerName, SDL_Point window_pos, Map& map);
    Window createWindow(SDL_Point window_pos);
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    SDL_Point getCenterPoint();
    void addShotEffect(Bullet bullet);
    void switchShopVisibility();

    void resizeHud();


    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> getWeaponShopButtons() { return weaponShopButtons; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuyPrimaryAmmoButton() { return buyPrimaryAmmoButton; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuySecondaryAmmoButton() { return buySecondaryAmmoButton; };

private:
    Mixer mixer = Mixer(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Window window;
    Renderer renderer;
    Game& game;
    std::string playerName;
    Map& map;


    Chunk glockSound = Chunk("../assets/sfx/weapons/usp_silenced.wav");

    Texture mapTiles;
    Texture backgroundTexture;
    Texture playerTiles = Texture(renderer, "../assets/gfx/player/ct1.bmp"); // temporal
    Font font = Font("../assets/gfx/fonts/sourcesans.ttf", FONT_SIZE);

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

    Texture bloodTexture;
    Texture createBloodTexture() {
            Surface surface(
                0, PARTICLE_SIZE, PARTICLE_SIZE, 32,
                0x00FF0000, // Rmask
                0x0000FF00, // Gmask
                0x000000FF, // Bmask
                0xFF000000  // Amask
            );
            surface.FillRect(SDL2pp::NullOpt,
                            SDL_MapRGBA(surface.Get()->format, 255, 0, 0, 255));
            return Texture(renderer, surface);
    }

    Texture sparkTexture;
    Texture createSparkTexture() {
            Surface surface(
                0, PARTICLE_SIZE, PARTICLE_SIZE, 32,
                0x00FF0000, // Rmask
                0x0000FF00, // Gmask
                0x000000FF, // Bmask
                0xFF000000  // Amask
            );
            surface.FillRect(SDL2pp::NullOpt,
                            SDL_MapRGBA(surface.Get()->format, 255, 240, 0, 255));
            
            
            return Texture(renderer, surface);
    }


    std::unordered_map<WeaponName, std::string> weaponTexts = {
        {AK47, "AK-47"},
        {M3, "M3"},
        {AWP, "AWP"},
        {GLOCK, "Glock"}
    };

    Surface akShopSprite = Surface("../assets/gfx/weapons/ak47_m.bmp");
    Surface m3ShopSprite = Surface("../assets/gfx/weapons/m3_m.bmp");
    Surface awpShopSprite = Surface("../assets/gfx/weapons/awp_m.bmp");
    Surface& getWeaponShopSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47: return akShopSprite;
            case M3:   return m3ShopSprite;
            case AWP:  return awpShopSprite;
            default:   throw std::exception();
        }
    }





    Surface akInvSprite = Surface("../assets/gfx/weapons/ak47_k.bmp");
    Surface m3InvSprite = Surface("../assets/gfx/weapons/m3_k.bmp");
    Surface awpInvSprite = Surface("../assets/gfx/weapons/awp_k.bmp");
    Surface glockInvSprite = Surface("../assets/gfx/weapons/glock_k.bmp");
    Surface knifeInvSprite = Surface("../assets/gfx/weapons/knife_k.bmp");
    Surface bombInvSprite = Surface("../assets/gfx/weapons/bomb_d.bmp");
    Surface& getWeaponInvSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akInvSprite;
            case M3:    return m3InvSprite;
            case AWP:   return awpInvSprite;
            case GLOCK: return glockInvSprite;
            case KNIFE: return knifeInvSprite;
            default:    throw std::exception();
        }
    }

    
    Surface akInvSprite = Surface("../assets/gfx/weapons/ak47_k.bmp");
    Surface m3InvSprite = Surface("../assets/gfx/weapons/m3_k.bmp");
    Surface awpInvSprite = Surface("../assets/gfx/weapons/awp_k.bmp");
    Surface glockInvSprite = Surface("../assets/gfx/weapons/glock_k.bmp");
    Surface knifeInvSprite = Surface("../assets/gfx/weapons/knife_k.bmp");
    Surface bombInvSprite = Surface("../assets/gfx/weapons/bomb_d.bmp");
    Surface& getWeaponInvSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akInvSprite;
            case M3:    return m3InvSprite;
            case AWP:   return awpInvSprite;
            case GLOCK: return glockInvSprite;
            case KNIFE: return knifeInvSprite;
            default:    throw std::exception();
        }
    }

    Texture akInGameSprite = Texture(renderer, "../assets/gfx/weapons/ak47.bmp");
    Texture m3InGameSprite = Texture(renderer, "../assets/gfx/weapons/m3.bmp");
    Texture awpInGameSprite = Texture(renderer, "../assets/gfx/weapons/awp.bmp");
    Texture glockInGameSprite = Texture(renderer, "../assets/gfx/weapons/glock.bmp");
    Texture knifeInGameSprite = Texture(renderer, "../assets/gfx/weapons/knife.bmp");
    Texture& getWeaponInGameSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akInGameSprite;
            case M3:    return m3InGameSprite;
            case AWP:   return awpInGameSprite;
            case GLOCK:   return glockInGameSprite;
            case KNIFE:   return knifeInGameSprite;
            default:    throw std::exception();
        }
    }


    Texture akDroppedSprite = Texture(renderer, "../assets/gfx/weapons/ak47_d.bmp");
    Texture m3DroppedSprite = Texture(renderer, "../assets/gfx/weapons/m3_d.bmp");
    Texture awpDroppedSprite = Texture(renderer, "../assets/gfx/weapons/awp_d.bmp");
    Texture& getWeaponDroppedSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akDroppedSprite;
            case M3:    return m3DroppedSprite;
            case AWP:   return awpDroppedSprite;
            default:    throw std::exception();
        }
    }

    bool shopIsVisible = false;
    std::vector<ShotEffect> shot_effects;

    std::vector<HitEffect> blood_effects;
    std::vector<HitEffect> sparks_effects;
    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> weaponShopButtons;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buySecondaryAmmoButton;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buyPrimaryAmmoButton;

    void showBackground();
    void showMap(float cameraX, float cameraY);
    void showBullets(float cameraX, float cameraY, float deltaTime);
    void showBloodEffects(float cameraX, float cameraY, float deltaTime);
    void showSparksEffects(float cameraX, float cameraY, float deltaTime);
    void showEntities(float cameraX, float cameraY);
    void showInterface();
    void showShop();

    float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }



};

#endif

