#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include <random>
#include "../../common/structures.h"

// ../assets/


#define CLIP_SIZE 32
#define BLOCK_SIZE 32
#define BACKGROUND_TEXTURE_SIZE 64

using namespace SDL2pp;

#define DEATH_DURATION 1
#define BULLET_SPEED 120
#define BULLET_THICKNESS 2
#define BULLET_LENGTH 8


#define FONT_SIZE 32


struct BulletEffect {
    float x;
    float y;
    float target_x;
    float target_y;
    float angle;
    Impact impact;
    float time_left = 1;
};

struct DeathEffect {
    float dead_body_x;
    float dead_body_y;
    float dead_body_rotation;
    std::reference_wrapper<Surface> dead_body_skin;
    float time_left = DEATH_DURATION;
    int alpha = 255;
};

#define FOV M_PI/2

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


#define FLARE_EFFECT_DURATION 0.5



struct BombExplosionEffect {
    float x, y;
    int current_frame = 0;
    float time_until_next_frame = 0.06;

};


struct OnScreenMessageEffect {
    std::string text;
    float time_left = 2;
};

struct Particle {
    float x, y;
    float speed;
    float angle;
    float time_left;
};

struct HitEffect {
    std::vector<Particle> particles;
};


class GameView {
public:
    GameView(const std::string& playerName, SDL_Point window_pos, Map& map, std::vector<WeaponInfo>& weapons, Shop& shop, std::vector<PlayerInfo>& players);
    Window createWindow(SDL_Point window_pos);
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    SDL_Point getCenterPoint();
    void addBulletEffects(Shot shot);
    void addDeathEffect(float x, float y, PlayerData& data);
    void addBombExplosionEffect(float x, float y);
    void showNewPhaseMessage(Phase phase);
    void showRoundEndMessage(RoundWinner winner);
    void switchShopVisibility();
    void hideShop() { shopIsVisible = false; };
    void switchFovVisibility() { fovIsVisible = !fovIsVisible; };
    void updateState(StateGame& new_state) { state = new_state; }
    void setPhaseTimer(float time) { phaseTimer = time; }

    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> getWeaponShopButtons() { return weaponShopButtons; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuyPrimaryAmmoButton() { return buyPrimaryAmmoButton; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuySecondaryAmmoButton() { return buySecondaryAmmoButton; };

private:
    Window window;
    Renderer renderer;
    StateGame state;
    std::string playerName;
    Map& map;
    std::vector<WeaponInfo>& weapons;
    Shop& shop;
    std::vector<PlayerInfo>& players;

    Texture mapTiles;
    Texture backgroundTexture;
    const std::string BASE_PATH = "/var/taller/";

    Font font = Font(BASE_PATH + "gfx/fonts/sourcesans.ttf", FONT_SIZE);

    // Surfaces de las skins para poder crear otras texturas, util para hacer las animaciones de muerte
    Surface phoenixS = Surface(BASE_PATH + "gfx/player/t1.bmp");
    Surface l337KrewS = Surface(BASE_PATH + "gfx/player/t2.bmp");
    Surface arcticAvengerS = Surface(BASE_PATH + "gfx/player/t3.bmp");
    Surface guerrillaS = Surface(BASE_PATH + "gfx/player/t4.bmp");
    Surface& getTSkinSpriteSurface(tSkin skin) {
        switch (skin) {
            case PHOENIX:           return phoenixS;
            case L337_KREW:         return l337KrewS;
            case ARCTIC_AVENGER:    return arcticAvengerS;
            case GUERRILLA:         return guerrillaS;
            default:                throw std::exception();
        }
    }
    Surface sealForceS = Surface(BASE_PATH + "gfx/player/ct1.bmp");
    Surface germanGsg9S = Surface(BASE_PATH + "gfx/player/ct2.bmp");
    Surface uksasS = Surface(BASE_PATH + "gfx/player/ct3.bmp");
    Surface frenchGignS = Surface(BASE_PATH + "gfx/player/ct4.bmp");
    Surface& getCtSkinSpriteSurface(ctSkin skin) {
        switch (skin) {
            case SEAL_FORCE:        return sealForceS;
            case GERMAN_GSG9:       return germanGsg9S;
            case UKSAS:             return uksasS;
            case FRENCH_GIGN:       return frenchGignS;
            default:                throw std::exception();
        }
    }

    // T skins
    Texture phoenix = Texture(renderer, phoenixS);
    Texture l337Krew = Texture(renderer, l337KrewS);
    Texture arcticAvenger = Texture(renderer, arcticAvengerS);
    Texture guerrilla = Texture(renderer, guerrillaS);
    Texture& getTSkinSprite(tSkin skin) {
        switch (skin) {
            case PHOENIX:           return phoenix;
            case L337_KREW:         return l337Krew;
            case ARCTIC_AVENGER:    return arcticAvenger;
            case GUERRILLA:         return guerrilla;
            default:                throw std::exception();
        }
    }
    // CT skins
    Texture sealForce = Texture(renderer, sealForceS);
    Texture germanGsg9 = Texture(renderer, germanGsg9S);
    Texture uksas = Texture(renderer, uksasS);
    Texture frenchGign = Texture(renderer, frenchGignS);
    Texture& getCtSkinSprite(ctSkin skin) {
        switch (skin) {
            case SEAL_FORCE:        return sealForce;
            case GERMAN_GSG9:       return germanGsg9;
            case UKSAS:             return uksas;
            case FRENCH_GIGN:       return frenchGign;
            default:                throw std::exception();
        }
    }


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

    Texture akShopSprite;
    Texture m3ShopSprite;
    Texture awpShopSprite;
    Texture createShopTexture(const std::string& path) {
        Surface ShopSpriteS = Surface(path);
        ShopSpriteS.SetColorKey(true, SDL_MapRGB(ShopSpriteS.Get()->format, 255, 0, 255));
        return Texture(renderer, ShopSpriteS);
    }
    Texture& getWeaponShopSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47: return akShopSprite;
            case M3:   return m3ShopSprite;
            case AWP:  return awpShopSprite;
            default:   throw std::exception();
        }
    }

    
    Texture akInvSprite;
    Texture m3InvSprite;
    Texture awpInvSprite;
    Texture glockInvSprite;
    Texture knifeInvSprite;
    Texture bombInvSprite;
    Texture createInvTexture(const std::string& path) {
        Surface invSpriteS = Surface(path);
        invSpriteS.SetColorKey(true, SDL_MapRGB(invSpriteS.Get()->format, 0, 0, 0));
        return Texture(renderer, invSpriteS);
    }
    Texture& getWeaponInvSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akInvSprite;
            case M3:    return m3InvSprite;
            case AWP:   return awpInvSprite;
            case GLOCK: return glockInvSprite;
            case KNIFE: return knifeInvSprite;
            default:    throw std::exception();
        }
    }

    Texture akInGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/ak47.bmp");
    Texture m3InGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/m3.bmp");
    Texture awpInGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/awp.bmp");
    Texture glockInGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/glock.bmp");
    Texture knifeInGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/knife.bmp");
    Texture bombInGameSprite = Texture(renderer, BASE_PATH + "gfx/weapons/bomb.bmp");
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


    Texture akDroppedSprite = Texture(renderer, BASE_PATH + "gfx/weapons/ak47_d.bmp");
    Texture m3DroppedSprite = Texture(renderer, BASE_PATH + "gfx/weapons/m3_d.bmp");
    Texture awpDroppedSprite = Texture(renderer, BASE_PATH + "gfx/weapons/awp_d.bmp");
    Texture& getWeaponDroppedSprite(WeaponName weapon) {
        switch (weapon) {
            case AK47:  return akDroppedSprite;
            case M3:    return m3DroppedSprite;
            case AWP:   return awpDroppedSprite;
            default:    throw std::exception();
        }
    }

    Texture bombAnimationSprite = Texture(renderer, BASE_PATH + "gfx/explosion.png");

    bool shopIsVisible = false;
    bool fovIsVisible = true;
    bool bombExplosionEffect = false;

    std::vector<HitEffect> blood_effects;
    std::vector<HitEffect> sparks_effects;
    std::vector<BulletEffect> bullet_effects;
    std::vector<DeathEffect> death_effects;
    OnScreenMessageEffect on_screen_message_effect = OnScreenMessageEffect{"Fase de compra"};
    BombExplosionEffect bomb_explosion_effect;
    float phaseTimer = 0;

    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> weaponShopButtons;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buySecondaryAmmoButton;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buyPrimaryAmmoButton;

    void showBackground();
    void showMap(float cameraX, float cameraY);
    void showBullets(float cameraX, float cameraY, float deltaTime);
    void showBloodEffects(float cameraX, float cameraY, float deltaTime);
    void showSparksEffects(float cameraX, float cameraY, float deltaTime);
    void showEntities(float cameraX, float cameraY);
    void showPlayer(float cameraX, float cameraY, Entity player);
    void showBomb(float cameraX, float cameraY, Entity bomb);
    void showWeapon(float cameraX, float cameraY, Entity weapon);
    void showDeathAnimations(float cameraX, float cameraY, float deltaTime);
    void showOnScreenMessage(float deltaTime);
    void showBombExplosion(float cameraX, float cameraY, float deltaTime);
    void showFov(float angle);


    void showInterface(Inventory inventory, WeaponType equippedWeapon, int health);
    void showShop(Inventory inventory, int money);

    float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    PlayerInfo findPlayerInfo(const std::string& name) {
        for (size_t i = 0; i < players.size(); i++) {
            if (players[i].name == name) {
                return players[i];
            }
        }
        return {};
    };
    WeaponInfo findWeaponInfo(WeaponName weapon) {
        for (size_t i = 0; i < weapons.size(); i++) {
            if (weapons[i].name == weapon) {
                return weapons[i];
            }
        }
        return {};
    };

};

#endif