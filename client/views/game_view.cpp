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
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

        AK47ShopSprite.SetColorKey(true, SDL_MapRGB(AK47ShopSprite.Get()->format, 255, 0, 255));
        M3ShopSprite.SetColorKey(true, SDL_MapRGB(M3ShopSprite.Get()->format, 255, 0, 255));
        AWPShopSprite.SetColorKey(true, SDL_MapRGB(AWPShopSprite.Get()->format, 255, 0, 255));
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
    //float cameraX = center.x - game.getX(playerName) * BLOCK_SIZE - BLOCK_SIZE/2; + (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2
    float cameraX = center.x - game.getX(playerName) * BLOCK_SIZE - BLOCK_SIZE/2 + (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2;
    float cameraY = center.y - game.getY(playerName) * BLOCK_SIZE - BLOCK_SIZE/2 + (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2;


    showBackground();
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
void GameView::showBackground() {
    renderer.SetDrawColor(0, 0, 0, 255);
    Rect src(0, 0, BACKGROUND_TEXTURE_SIZE, BACKGROUND_TEXTURE_SIZE);

    int screenWidth = window.GetWidth();
    int screenHeight = window.GetHeight();

    for (int y = 0; y < screenHeight; y +=  BACKGROUND_TEXTURE_SIZE) {
        for (int x = 0; x < screenWidth; x +=  BACKGROUND_TEXTURE_SIZE) {
            Rect dst(x, y, BACKGROUND_TEXTURE_SIZE, BACKGROUND_TEXTURE_SIZE);
            renderer.Copy(backgroundTexture, src, dst);
        }
    }
}
void GameView::showMap(float cameraX, float cameraY) {
    renderer.SetDrawColor(0, 0, 0, 255);
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
    renderer.SetDrawColor(0, 0, 0, 255);
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
    renderer.SetDrawColor(0, 0, 0, 255);
    std::vector<Entity> gameState = game.getState().entities;
    Rect src(0, 0, CLIP_SIZE, CLIP_SIZE); // temporal, hasta que definamos bien como se deberian ver los jugadores
    for (size_t i = 0; i < gameState.size(); i++) {
        switch (gameState[i].type) {
            case PLAYER: {
                PlayerData data = std::get<PlayerData>(gameState[i].data);
                float playerX = gameState[i].x;
                float playerY = gameState[i].y;

                Rect dst(cameraX + playerX * BLOCK_SIZE - (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2, cameraY + playerY * BLOCK_SIZE - (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE);
                renderer.Copy(playerTiles, src, dst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
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
    const int ITEM_CONTAINER_MARGIN = 10;

    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(shopLayout.container);
    
    auto shop = game.getStore();

    Surface primaryWeaponSectionLabel = font.RenderText_Blended("PRIMARY WEAPONS", Color(255, 255, 255));
    Texture primaryWeaponSectionLabelTexture(renderer, primaryWeaponSectionLabel);
    renderer.Copy(
        primaryWeaponSectionLabelTexture, 
        NullOpt, 
        shopLayout.primaryWeaponLabel);

    renderer.SetDrawColor(255, 255, 255, 32);


    Surface BoughtLabel = font.RenderText_Blended("Comprada", Color(255, 255, 255));
    Texture BoughtLabelTexture(renderer, BoughtLabel);

    Entity player = game.getPlayerState(playerName);
    Inventory inv = std::get<PlayerData>(player.data).inventory;
    
    //int item_container_x = container.GetX() + CONTAINER_MARGIN;
    for (size_t i = 0; i < shop.size(); i++) {
        WeaponName weapon = shop[i].first;
        int price = shop[i].second;
        std::string weaponLabelText;

        Texture weaponTexture(renderer, AK47ShopSprite);
        switch (weapon) {
            case WeaponName::AK47: {
                weaponLabelText = "AK-47";
                break;
            }
            case WeaponName::AWP: {
                weaponLabelText = "AWP";
                weaponTexture = Texture(renderer, AWPShopSprite);
                break;
            }
            case WeaponName::M3: {
                weaponLabelText = "M3";
                weaponTexture = Texture(renderer, M3ShopSprite);
                break;
            }
            default: {
                break;
            }

        }

        if (inv.primary == weapon) {
            renderer.SetDrawColor(255, 255, 255, 64);
        } else {
            renderer.SetDrawColor(255, 255, 255, 32);
        }
        renderer.FillRect(shopLayout.weaponItemContainers[i]);

        Surface weaponLabel = font.RenderText_Blended(weaponLabelText, Color(255, 255, 255));
        Texture weaponLabelTexture(renderer, weaponLabel);
        Surface weaponPriceLabel = font.RenderText_Blended("$" + std::to_string(price), Color(255, 255, 255));
        Texture weaponPriceLabelTexture(renderer, weaponPriceLabel);
        renderer.Copy(
            weaponLabelTexture, 
            NullOpt, 
            shopLayout.weaponLabels[i]);

        renderer.Copy(
            weaponPriceLabelTexture, 
            NullOpt, 
            shopLayout.weaponPriceLabels[i]);

        
        if (inv.primary == weapon) {
            renderer.Copy(
                BoughtLabelTexture, 
                NullOpt, 
                shopLayout.boughtLabels[i]);
        }

        renderer.Copy(
            weaponTexture, 
            NullOpt, 
            shopLayout.weaponSprites[i]);
    }


    Surface ammoSectionLabel = font.RenderText_Blended("AMMO", Color(255, 255, 255));
    Texture ammoSectionLabelTexture(renderer, ammoSectionLabel);
    renderer.Copy(ammoSectionLabelTexture, NullOpt, shopLayout.ammoLabel);

    if (inv.bulletsPrimary >= Weapons::getWeapon(inv.primary).maxAmmo) {
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(255, 255, 255, 32);
    }
    renderer.FillRect(shopLayout.primaryAmmoContainer);

    if (inv.bulletsSecondary >= Weapons::getWeapon(inv.secondary).maxAmmo) {
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(255, 255, 255, 32);
    }
    renderer.FillRect(shopLayout.secondaryAmmoContainer);


    Surface primaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsPrimary) + "/" + std::to_string(Weapons::getWeapon(inv.primary).maxAmmo), Color(255, 255, 255));
    Texture primaryAmmoBoughtLabelTexture(renderer, primaryAmmoBoughtLabel);

    Surface primaryAmmoLabel = font.RenderText_Blended("Primary", Color(255, 255, 255));
    Texture primaryAmmoLabelTexture(renderer, primaryAmmoLabel);
    Surface primaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    Texture primaryAmmoPriceLabelTexture(renderer, primaryAmmoPriceLabel);
    renderer.Copy(primaryAmmoLabelTexture, NullOpt, shopLayout.primaryAmmoLabel);
    renderer.Copy(primaryAmmoPriceLabelTexture, NullOpt, shopLayout.primaryAmmoPriceLabel);

    renderer.Copy(
        primaryAmmoBoughtLabelTexture, 
        NullOpt, 
        Rect(
            shopLayout.primaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN + ((shopLayout.primaryAmmoContainer.GetW() - primaryAmmoBoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), 
            shopLayout.primaryAmmoContainer.GetY() + shopLayout.primaryAmmoContainer.GetH() - primaryAmmoBoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, 
            primaryAmmoBoughtLabelTexture.GetWidth(), 
            primaryAmmoBoughtLabelTexture.GetHeight()));

    Surface secondaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsSecondary) + "/" + std::to_string(Weapons::getWeapon(inv.secondary).maxAmmo), Color(255, 255, 255));
    Texture secondaryAmmoBoughtLabelTexture(renderer, secondaryAmmoBoughtLabel);

    Surface secondaryAmmoLabel = font.RenderText_Blended("Second.", Color(255, 255, 255));
    Texture secondaryAmmoLabelTexture(renderer, secondaryAmmoLabel);
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    Texture secondaryAmmoPriceLabelTexture(renderer, secondaryAmmoPriceLabel);
    renderer.Copy(secondaryAmmoLabelTexture, NullOpt, shopLayout.secondaryAmmoLabel); // Rect(seconary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN, seconary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, secondaryAmmoLabelTexture.GetWidth(), secondaryAmmoLabelTexture.GetHeight()));
    renderer.Copy(secondaryAmmoPriceLabelTexture, NullOpt, shopLayout.secondaryAmmoPriceLabel); // Rect(seconary_ammo_container.GetX() + seconary_ammo_container.GetW() - secondaryAmmoPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, seconary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, secondaryAmmoPriceLabelTexture.GetWidth(), secondaryAmmoPriceLabelTexture.GetHeight()));

    renderer.Copy(
        secondaryAmmoBoughtLabelTexture, 
        NullOpt, 
        Rect(
            shopLayout.secondaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN + ((shopLayout.secondaryAmmoContainer.GetW() - secondaryAmmoBoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), 
            shopLayout.secondaryAmmoContainer.GetY() + shopLayout.secondaryAmmoContainer.GetH() - secondaryAmmoBoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, 
            secondaryAmmoBoughtLabelTexture.GetWidth(), 
            secondaryAmmoBoughtLabelTexture.GetHeight()));

}

ShopLayout GameView::createShopLayout() {

    ShopLayout layout;

    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();
    const int MARGIN = 40;
    const int CONTAINER_MARGIN = 20;
    const int SECTION_VERTICAL_SPACING = 10; // el spacing que hay entre el label Primary Weapon y los botones de las armas, mismo para la seccion de ammo
    const int SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING = 20;
    const int CONTAINER_VERTICAL_SPACING = 20;
    const int ITEM_CONTAINER_MARGIN = 10;
    const int ITEM_CONTAINER_VERTICAL_SPACING = 5;

    
    Rect container(MARGIN, MARGIN, width - 2 * MARGIN, height - 2 * MARGIN);
    layout.container = container;
    
    int item_container_hbox_width = container.GetW() - CONTAINER_MARGIN * 2;


    auto shop = game.getStore();


    
    Surface primaryWeaponSectionLabel = font.RenderText_Blended("PRIMARY WEAPONS", Color(255, 255, 255));
    //Texture primaryWeaponSectionLabelTexture(renderer, primaryWeaponSectionLabel);

    Rect primaryWeaponSectionLabelRect(
            container.GetX() + CONTAINER_MARGIN, 
            container.GetY() + CONTAINER_MARGIN, 
            primaryWeaponSectionLabel.GetWidth(), 
            primaryWeaponSectionLabel.GetHeight());
    
    layout.primaryWeaponLabel = primaryWeaponSectionLabelRect;


    Surface BoughtLabel = font.RenderText_Blended("Comprada", Color(255, 255, 255));
    //Texture BoughtLabelTexture(renderer, BoughtLabel);

    // Entity player = game.getPlayerState(playerName);
    // Inventory inv = std::get<PlayerData>(player.data).inventory;
    
    int item_container_x = container.GetX() + CONTAINER_MARGIN;
    for (size_t i = 0; i < shop.size(); i++) {
        WeaponName weapon = shop[i].first;
        int price = shop[i].second;
        std::string weaponLabelText;

        //Texture weaponTexture(renderer, AK47ShopSprite);
        switch (weapon) {
            case WeaponName::AK47: {
                weaponLabelText = "AK-47";
                break;
            }
            case WeaponName::AWP: {
                weaponLabelText = "AWP";
                //weaponTexture = Texture(renderer, AWPShopSprite);
                break;
            }
            case WeaponName::M3: {
                weaponLabelText = "M3";
                //weaponTexture = Texture(renderer, M3ShopSprite);
                break;
            }
            default: {
                break;
            }

        }

        Rect item_container(
            item_container_x, 
            container.GetY() + CONTAINER_MARGIN + primaryWeaponSectionLabel.GetHeight() + SECTION_VERTICAL_SPACING, 
            (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.size() - 1))/3, 
            container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabel.GetHeight() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
        layout.weaponItemContainers.push_back(item_container);

        weaponShopButtons[weapon] = {{item_container.GetX(), item_container.GetY()}, {item_container.GetW(), item_container.GetH()}};
        item_container_x = item_container.GetX() + item_container.GetW() + SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING;



    

        Surface weaponLabel = font.RenderText_Blended(weaponLabelText, Color(255, 255, 255));
        Rect weaponLabelRect(
                item_container.GetX() + ITEM_CONTAINER_MARGIN, 
                item_container.GetY() + ITEM_CONTAINER_MARGIN, 
                weaponLabel.GetWidth(), 
                weaponLabel.GetHeight());
        layout.weaponLabels.push_back(weaponLabelRect);
        // Texture weaponLabelTexture(renderer, weaponLabel);
        Surface weaponPriceLabel = font.RenderText_Blended("$" + std::to_string(price), Color(255, 255, 255));
        Rect weaponPriceLabelRect(
                item_container.GetX() + item_container.GetW() - weaponPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, 
                item_container.GetY() + ITEM_CONTAINER_MARGIN, 
                weaponPriceLabel.GetWidth(), 
                weaponPriceLabel.GetHeight());
        layout.weaponPriceLabels.push_back(weaponPriceLabelRect);
        // Texture weaponPriceLabelTexture(renderer, weaponPriceLabel);
        
        Rect weaponTextureRect(
            item_container.GetX() + ITEM_CONTAINER_MARGIN, 
            item_container.GetY() + ITEM_CONTAINER_MARGIN + weaponLabel.GetHeight() + ITEM_CONTAINER_VERTICAL_SPACING, 
            item_container.GetW() - ITEM_CONTAINER_MARGIN * 2, 
            item_container.GetH() - ITEM_CONTAINER_MARGIN * 2 - ITEM_CONTAINER_VERTICAL_SPACING * 2 - weaponLabel.GetHeight() * 2);
        layout.weaponSprites.push_back(weaponTextureRect);


        Rect boughtLabelRect(
            item_container.GetX() + ITEM_CONTAINER_MARGIN + ((item_container.GetW() - BoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), 
            item_container.GetY() + item_container.GetH() - BoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, 
            BoughtLabel.GetWidth(), 
            BoughtLabel.GetHeight());
        layout.boughtLabels.push_back(boughtLabelRect);
        
    }


    Surface ammoSectionLabel = font.RenderText_Blended("AMMO", Color(255, 255, 255));
    //Texture ammoSectionLabelTexture(renderer, ammoSectionLabel);
    Rect ammoSectionLabelRect(
        container.GetX() + CONTAINER_MARGIN, 
        container.GetY() + container.GetH() / 2 + CONTAINER_VERTICAL_SPACING / 2, 
        ammoSectionLabel.GetWidth(), 
        ammoSectionLabel.GetHeight());
    layout.ammoLabel = ammoSectionLabelRect;

    Rect primary_ammo_container(
        container.GetX() + CONTAINER_MARGIN, 
        container.GetY() + container.GetH() / 2 + CONTAINER_VERTICAL_SPACING / 2 + ammoSectionLabel.GetHeight() + SECTION_VERTICAL_SPACING, 
        (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.size() - 1))/3, 
        container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabel.GetHeight() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
    layout.primaryAmmoContainer = primary_ammo_container;

    Rect secondary_ammo_container(
        primary_ammo_container.GetX() + primary_ammo_container.GetW() + SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING,
        primary_ammo_container.GetY(), 
        primary_ammo_container.GetW(), 
        primary_ammo_container.GetH());
    layout.secondaryAmmoContainer = secondary_ammo_container;


    buyPrimaryAmmoButton = {{primary_ammo_container.GetX(), primary_ammo_container.GetY()}, {primary_ammo_container.GetW(), primary_ammo_container.GetH()}};
    buySecondaryAmmoButton = {{secondary_ammo_container.GetX(), secondary_ammo_container.GetY()}, {secondary_ammo_container.GetW(), secondary_ammo_container.GetH()}};
    // Surface primaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsPrimary) + "/" + std::to_string(Weapons::getWeapon(inv.primary).maxAmmo), Color(255, 255, 255));
    // Texture primaryAmmoBoughtLabelTexture(renderer, primaryAmmoBoughtLabel);

    Surface primaryAmmoLabel = font.RenderText_Blended("Primary", Color(255, 255, 255));
    // Texture primaryAmmoLabelTexture(renderer, primaryAmmoLabel);
    Surface primaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    // Texture primaryAmmoPriceLabelTexture(renderer, primaryAmmoPriceLabel);
    // renderer.Copy(primaryAmmoLabelTexture, NullOpt, Rect(primary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN, primary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, primaryAmmoLabelTexture.GetWidth(), primaryAmmoLabelTexture.GetHeight()));
    // renderer.Copy(primaryAmmoPriceLabelTexture, NullOpt, Rect(primary_ammo_container.GetX() + primary_ammo_container.GetW() - primaryAmmoPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, primary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, primaryAmmoPriceLabelTexture.GetWidth(), primaryAmmoPriceLabelTexture.GetHeight()));

    Rect primaryAmmoLabelRect(
        primary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN, 
        primary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoLabel.GetWidth(), 
        primaryAmmoLabel.GetHeight());
    layout.primaryAmmoLabel = primaryAmmoLabelRect;

    Rect primaryAmmoPriceLabelRect(
        primary_ammo_container.GetX() + primary_ammo_container.GetW() - primaryAmmoPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, 
        primary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoPriceLabel.GetWidth(), 
        primaryAmmoPriceLabel.GetHeight());
    layout.primaryAmmoPriceLabel = primaryAmmoPriceLabelRect;

    // renderer.Copy(primaryAmmoBoughtLabelTexture, NullOpt, Rect(primary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN + ((primary_ammo_container.GetW() - primaryAmmoBoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), primary_ammo_container.GetY() + primary_ammo_container.GetH() - primaryAmmoBoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, primaryAmmoBoughtLabelTexture.GetWidth(), primaryAmmoBoughtLabelTexture.GetHeight()));

    // Surface secondaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsSecondary) + "/" + std::to_string(Weapons::getWeapon(inv.secondary).maxAmmo), Color(255, 255, 255));
    // Texture secondaryAmmoBoughtLabelTexture(renderer, secondaryAmmoBoughtLabel);

    Surface secondaryAmmoLabel = font.RenderText_Blended("Second.", Color(255, 255, 255));
    // Texture secondaryAmmoLabelTexture(renderer, secondaryAmmoLabel);
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    // Texture secondaryAmmoPriceLabelTexture(renderer, secondaryAmmoPriceLabel);
    // renderer.Copy(secondaryAmmoLabelTexture, NullOpt, Rect(seconary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN, seconary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, secondaryAmmoLabelTexture.GetWidth(), secondaryAmmoLabelTexture.GetHeight()));
    // renderer.Copy(secondaryAmmoPriceLabelTexture, NullOpt, Rect(seconary_ammo_container.GetX() + seconary_ammo_container.GetW() - secondaryAmmoPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, seconary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, secondaryAmmoPriceLabelTexture.GetWidth(), secondaryAmmoPriceLabelTexture.GetHeight()));

    // renderer.Copy(secondaryAmmoBoughtLabelTexture, NullOpt, Rect(seconary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN + ((seconary_ammo_container.GetW() - secondaryAmmoBoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), seconary_ammo_container.GetY() + seconary_ammo_container.GetH() - secondaryAmmoBoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, secondaryAmmoBoughtLabelTexture.GetWidth(), secondaryAmmoBoughtLabelTexture.GetHeight()));
    Rect secondaryAmmoLabelRect(
        secondary_ammo_container.GetX() + ITEM_CONTAINER_MARGIN, 
        secondary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoLabel.GetWidth(), 
        secondaryAmmoLabel.GetHeight());
    layout.secondaryAmmoLabel = secondaryAmmoLabelRect;

    Rect secondaryAmmoPriceLabelRect(
        secondary_ammo_container.GetX() + secondary_ammo_container.GetW() - secondaryAmmoPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, 
        secondary_ammo_container.GetY() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoPriceLabel.GetWidth(), 
        secondaryAmmoPriceLabel.GetHeight());
    layout.secondaryAmmoPriceLabel = secondaryAmmoPriceLabelRect;
    return layout;
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