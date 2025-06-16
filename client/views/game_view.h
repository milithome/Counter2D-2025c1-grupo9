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


#define DEATH_DURATION 1
#define BULLET_SPEED 120
#define BULLET_DURATION 1
#define BULLET_THICKNESS 2
#define BULLET_LENGTH 8


#define FONT_SIZE 32


struct BulletEffect {
    float x;
    float y;
    float target_x;
    float target_y;
    float angle;
    float time_left;
    Impact impact;
};

struct DeathEffect {
    float dead_body_x;
    float dead_body_y;
    float dead_body_rotation;
    //Surface dead_body_skin;
    float time_left;
    int alpha;
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

struct NewPhaseEffect {
    Phase phase;
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

enum Skin {
    PHOENIX,
    L337_KREW, 
    ARCTIC_AVENGER,
    GUERRILLA,
    SEAL_FORCE,
    GERMAN_GSG9,
    UKSAS,
    FRENCH_GIGN
};


// d inventory, dropped
// m shop
// _ ingame

class GameView {
public:
    GameView(const std::string& playerName, SDL_Point window_pos, Map& map);
    Window createWindow(SDL_Point window_pos);
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    SDL_Point getCenterPoint();
    void addBulletEffects(Shot shot);
    void addDeathEffect(float x, float y, float angle);
    void addBombExplosionEffect(float x, float y);
    void addNewPhaseEffect(Phase phase);
    void switchShopVisibility();
    void hideShop() { shopIsVisible = false; };
    void switchFovVisibility() { fovIsVisible = !fovIsVisible; };
    void resizeHud();
    void updateState(StateGame& new_state) {
        state = new_state;
    }

    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> getWeaponShopButtons() { return weaponShopButtons; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuyPrimaryAmmoButton() { return buyPrimaryAmmoButton; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuySecondaryAmmoButton() { return buySecondaryAmmoButton; };

private:
    Window window;
    Renderer renderer;
    //Game& game;
    StateGame state;
    std::string playerName;
    Map& map;



    Texture mapTiles;
    Texture backgroundTexture;
    Texture playerTiles = Texture(renderer, "../assets/gfx/player/ct1.bmp"); // temporal
    Font font = Font("../assets/gfx/fonts/sourcesans.ttf", FONT_SIZE);

    // Surfaces de las skins para poder crear otras texturas, util para hacer las animaciones de muerte
    Surface phoenixS = Surface("../assets/gfx/player/t1.bmp");
    Surface l337KrewS = Surface("../assets/gfx/player/t2.bmp");
    Surface arcticAvengerS = Surface("../assets/gfx/player/t3.bmp");
    Surface guerrillaS = Surface("../assets/gfx/player/t4.bmp");
    Surface sealForceS = Surface("../assets/gfx/player/ct1.bmp");
    Surface germanGsg9S = Surface("../assets/gfx/player/ct2.bmp");
    Surface uksasS = Surface("../assets/gfx/player/ct3.bmp");
    Surface frenchGignS = Surface("../assets/gfx/player/ct4.bmp");
    Surface& getSkinSpriteSurface(Skin skin) {
        switch (skin) {
            case PHOENIX:           return phoenixS;
            case L337_KREW:         return l337KrewS;
            case ARCTIC_AVENGER:    return arcticAvengerS;
            case GUERRILLA:         return guerrillaS;
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

    // CT skins
    Texture sealForce = Texture(renderer, sealForceS);
    Texture germanGsg9 = Texture(renderer, germanGsg9S);
    Texture uksas = Texture(renderer, uksasS);
    Texture frenchGign = Texture(renderer, frenchGignS);
    Texture& getSkinSprite(Skin skin) {
        switch (skin) {
            case PHOENIX:           return phoenix;
            case L337_KREW:         return l337Krew;
            case ARCTIC_AVENGER:    return arcticAvenger;
            case GUERRILLA:         return guerrilla;
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

    Texture akInGameSprite = Texture(renderer, "../assets/gfx/weapons/ak47.bmp");
    Texture m3InGameSprite = Texture(renderer, "../assets/gfx/weapons/m3.bmp");
    Texture awpInGameSprite = Texture(renderer, "../assets/gfx/weapons/awp.bmp");
    Texture glockInGameSprite = Texture(renderer, "../assets/gfx/weapons/glock.bmp");
    Texture knifeInGameSprite = Texture(renderer, "../assets/gfx/weapons/knife.bmp");
    Texture bombInGameSprite = Texture(renderer, "../assets/gfx/weapons/bomb.bmp");
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


    Surface buyPhaseLabel =  font.RenderText_Blended("Fase de compra", Color(255, 255, 255));
    Surface roundStartLabel = font.RenderText_Blended("Inicio de ronda", Color(255, 255, 255));
    Surface bombPlantedLabel =  font.RenderText_Blended("Bomba plantada", Color(255, 255, 255));
    Surface roundEndLabel =  font.RenderText_Blended("Fin de ronda", Color(255, 255, 255));
    Surface& getPhaseLabel(Phase phase) {
        switch (phase) {
            case PURCHASE:          return buyPhaseLabel;
            case BOMB_PLANTING:     return roundStartLabel;
            case BOMB_DEFUSING:     return bombPlantedLabel;
            case END_ROUND:         return roundEndLabel;
            default:                throw std::exception();
        }
    }

    Texture bombAnimationSprite = Texture(renderer, "../assets/gfx/explosion.png");

    bool shopIsVisible = false;
    bool fovIsVisible = true;
    bool bombExplosionEffect = false;

    std::vector<HitEffect> blood_effects;
    std::vector<HitEffect> sparks_effects;
    std::vector<BulletEffect> bullet_effects;
    std::vector<DeathEffect> death_effects;
    NewPhaseEffect new_phase_effect = NewPhaseEffect{PURCHASE};
    BombExplosionEffect bomb_explosion_effect;
    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> weaponShopButtons;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buySecondaryAmmoButton;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buyPrimaryAmmoButton;

    void showBackground();
    void showMap(float cameraX, float cameraY);
    void showBullets(float cameraX, float cameraY, float deltaTime);
    void showBloodEffects(float cameraX, float cameraY, float deltaTime);
    void showSparksEffects(float cameraX, float cameraY, float deltaTime);
    void showEntities(float cameraX, float cameraY);
    void showDeathAnimations(float cameraX, float cameraY, float deltaTime);
    void showNewPhase(float deltaTime);
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


};

#endif

