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



struct BulletEffect {
    float x;
    float y;
    float target_x;
    float target_y;
    float angle;
    float time_left;
};

struct DeathEffect {
    float dead_body_x;
    float dead_body_y;
    float dead_body_rotation;
    //Surface dead_body_skin;
    float time_left;
    int alpha = 255;
};

// Delirio
//////////////////////////////////////////////////////////////////////////
/*
struct ShopLayout {
    SdlContainer container;
    std::vector<SdlContainer> weaponItemContainers;
    std::vector<SdlSurfaceWidget> weaponLabels;
    std::vector<SdlSurfaceWidget> weaponPriceLabels;
    std::vector<SdlSurfaceWidget> boughtLabels;
    std::vector<SdlSurfaceWidget> weaponSprites;
    SdlContainer primaryAmmoContainer;
    SdlSurfaceWidget primaryAmmoLabel;
    SdlSurfaceWidget primaryAmmoPriceLabel;
    SdlSurfaceWidget primaryAmmoBoughtLabel;
    SdlContainer secondaryAmmoContainer;
    SdlSurfaceWidget secondaryAmmoLabel;
    SdlSurfaceWidget secondaryAmmoPriceLabel;
    SdlSurfaceWidget secondaryAmmoBoughtLabel;
}
*/
///////////////////////////////////////////////////////////////////////////
struct ShopLayout {
    Rect container;
    Rect primaryWeaponLabel;
    std::vector<Rect> weaponItemContainers;
    std::vector<Rect> weaponLabels;
    std::vector<Rect> weaponPriceLabels;
    std::vector<Rect> boughtLabels;
    std::vector<Rect> weaponSprites;

    Rect ammoLabel;
    Rect primaryAmmoContainer;
    Rect secondaryAmmoContainer;
    Rect primaryAmmoLabel;
    Rect secondaryAmmoLabel;
    Rect primaryAmmoPriceLabel;
    Rect secondaryAmmoPriceLabel;
};


struct InterfaceLayout {
    Rect container;
    std::function<Rect(Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position)> health;
    std::function<Rect(Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position)> ammo;
    std::function<Rect(Rect parent, Surface& label)> time;

    std::function<Rect(Rect parent, std::vector<Rect> parentsChildren, uint32_t position)> createWeaponContainer;
    std::function<Rect(Rect parent, Surface& sprite)> createWeaponSprite;

};

// TODO: cambiar el createShopLayout para que use esto
// struct ShopLayout {
//     Rect container;

//     std::function<Rect(std::vector<Rect> children)> sectionVBox;
//     std::function<Rect(Surface label, std::vector<Rect> parentsChildren, uint32_t position)> primaryWeaponLabel;
//     std::vector<std::function<Rect(std::vector<Rect> parentsChildren, uint32_t position)>> weaponItemContainers; // Es una HBox
//     ////////////////////////////////////////////////////////

//     // Forman parte de una "VBox" que esta adentro de cada itemContainer
//     std::vector<std::function<Rect(Surface label, std::vector<Rect> parentsChildren, uint32_t position)>> weaponLabels;
//     std::vector<std::function<Rect(Surface label, std::vector<Rect> parentsChildren, uint32_t position)>> weaponPriceLabels;
//     std::vector<std::function<Rect(Surface label, std::vector<Rect> parentsChildren, uint32_t position)>> weaponSprites;
//     std::vector<std::function<Rect(Surface label, std::vector<Rect> parentsChildren, uint32_t position)>> boughtLabels;
//     /////////////////////////////////////////////////////////////////////

//     std::function<Rect()> ammoLabel;
//     std::function<Rect()> primaryAmmoContainer;
//     std::function<Rect()> secondaryAmmoContainer;
//     std::function<Rect()> primaryAmmoLabel;
//     std::function<Rect()> secondaryAmmoLabel;
//     std::function<Rect()> primaryAmmoPriceLabel;
//     std::function<Rect()> secondaryAmmoPriceLabel;
//     std::function<Rect()> primaryAmmoBoughtLabel;
//     std::function<Rect()> secondaryAmmoBoughtLabel;
// };

// d inventory, dropped
// m shop
// _ ingame

class GameView {
public:
    //GameView(Game& game, const std::string& playerName, SDL_Point window_pos, const std::string& background_path, const std::string& sprite_path, const std::vector<std::vector<uint16_t>>& tiles_map, const std::unordered_map<uint16_t, MapLegendEntry>& legend_tiles);
    GameView(Game& game, const std::string& playerName, SDL_Point window_pos, Map& map);
    Window createWindow(SDL_Point window_pos);
    Renderer createRenderer(Window& window);
    void update(float deltaTime);
    SDL_Point getCenterPoint();
    void addBulletEffects(Shot shot);
    void addDeathEffect(float x, float y, float angle);
    void switchShopVisibility();

    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> getWeaponShopButtons() { return weaponShopButtons; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuyPrimaryAmmoButton() { return buyPrimaryAmmoButton; };
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> getBuySecondaryAmmoButton() { return buySecondaryAmmoButton; };

private:

    Window window;
    Renderer renderer;
    Game& game;
    std::string playerName;
    Map& map;

    Texture mapTiles;
    Texture backgroundTexture;
    Surface playerTiles = Surface("../assets/gfx/player/ct1.bmp"); // temporal
    Font font = Font("../assets/gfx/fonts/sourcesans.ttf", 20);

    // T skins
    Surface phoenix = Surface("../assets/gfx/player/t1.bmp");
    Surface l337Krew = Surface("../assets/gfx/player/t2.bmp");
    Surface arcticAvenger = Surface("../assets/gfx/player/t3.bmp");
    Surface guerrilla = Surface("../assets/gfx/player/t4.bmp");

    // CT skins
    Surface sealForce = Surface("../assets/gfx/player/ct1.bmp");
    Surface germanGSG9 = Surface("../assets/gfx/player/ct2.bmp");
    Surface UKSAS = Surface("../assets/gfx/player/ct3.bmp");
    Surface frenchGIGN = Surface("../assets/gfx/player/ct4.bmp");


    Surface AK47ShopSprite = Surface("../assets/gfx/weapons/ak47_m.bmp");
    Surface M3ShopSprite = Surface("../assets/gfx/weapons/m3_m.bmp");
    Surface AWPShopSprite = Surface("../assets/gfx/weapons/awp_m.bmp");

    Surface AKInvSprite = Surface("../assets/gfx/weapons/ak47_k.bmp");
    Surface M3InvSprite = Surface("../assets/gfx/weapons/m3_k.bmp");
    Surface AWPInvSprite = Surface("../assets/gfx/weapons/awp_k.bmp");

    Surface glockInvSprite = Surface("../assets/gfx/weapons/glock_k.bmp");
    Surface knifeInvSprite = Surface("../assets/gfx/weapons/knife_k.bmp");
    Surface bombInvSprite = Surface("../assets/gfx/weapons/bomb_d.bmp");

    Texture AKInGameSprite = Texture(renderer, "../assets/gfx/weapons/ak47.bmp");
    Texture M3InGameSprite = Texture(renderer, "../assets/gfx/weapons/m3.bmp");
    Texture AWPInGameSprite = Texture(renderer, "../assets/gfx/weapons/awp.bmp");

    bool shopIsVisible = false;
    std::vector<BulletEffect> bullet_effects;
    std::vector<DeathEffect> death_effects;
    std::unordered_map<WeaponName, std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>>> weaponShopButtons;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buySecondaryAmmoButton;
    std::pair<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> buyPrimaryAmmoButton;

    void showBackground();
    void showMap(float cameraX, float cameraY);
    void showBullets(float cameraX, float cameraY, float deltaTime);
    void showEntities(float cameraX, float cameraY);
    void showDeathAnimations(float cameraX, float cameraY, float deltaTime);

    void showInterface();
    void showShop();
    ShopLayout createShopLayout();
    InterfaceLayout createInterfaceLayout();

    ShopLayout shopLayout = createShopLayout();
    InterfaceLayout interfaceLayout = createInterfaceLayout();




};

#endif

