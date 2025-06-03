#include "game_view.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include "components/sdl_components/sdl_container.h"
#include "components/sdl_components/sdl_hboxcontainer.h"
#include "components/sdl_components/sdl_vboxcontainer.h"
#include "components/sdl_components/sdl_surfacewidget.h"



namespace fs = std::filesystem;



GameView::GameView(Game& game, const std::string& playerName, SDL_Point window_pos, Map& map)
    : window(createWindow(window_pos)), renderer(createRenderer(window)), game(game), playerName(playerName), map(map), mapTiles(Texture(renderer, map.get_sprite_path())), backgroundTexture(renderer, map.get_background_path()) {
        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

        mixer.SetVolume(-1, MIX_MAX_VOLUME); 

        AK47ShopSprite.SetColorKey(true, SDL_MapRGB(AK47ShopSprite.Get()->format, 255, 0, 255));
        M3ShopSprite.SetColorKey(true, SDL_MapRGB(M3ShopSprite.Get()->format, 255, 0, 255));
        AWPShopSprite.SetColorKey(true, SDL_MapRGB(AWPShopSprite.Get()->format, 255, 0, 255));

        AKInvSprite.SetColorKey(true, SDL_MapRGB(AKInvSprite.Get()->format, 0, 0, 0));
        M3InvSprite.SetColorKey(true, SDL_MapRGB(M3InvSprite.Get()->format, 0, 0, 0));
        AWPInvSprite.SetColorKey(true, SDL_MapRGB(AWPInvSprite.Get()->format, 0, 0, 0));
        glockInvSprite.SetColorKey(true, SDL_MapRGB(glockInvSprite.Get()->format, 0, 0, 0));
        knifeInvSprite.SetColorKey(true, SDL_MapRGB(knifeInvSprite.Get()->format, 0, 0, 0));
        bombInvSprite.SetColorKey(true, SDL_MapRGB(bombInvSprite.Get()->format, 0, 0, 0));
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
    float cameraX = center.x - game.getX(playerName) * BLOCK_SIZE - BLOCK_SIZE/2 + (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2;
    float cameraY = center.y - game.getY(playerName) * BLOCK_SIZE - BLOCK_SIZE/2 + (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2;


    showBackground();
    showMap(cameraX, cameraY);
    showBullets(cameraX, cameraY, deltaTime);
    showEntities(cameraX, cameraY);

    if (!shopIsVisible) {
        showInterface();
    }
    if (shopIsVisible) {
        showShop();
    }

    // TODO: sonido
    // Chunk sound("../assets/sfx/weapons/usp_silenced.wav");
    // mixer.PlayChannel(-1, sound, 0);

    renderer.Present();
}


void GameView::playShotSound() {
    std::cout << "sonido" << std::endl;
    mixer.PlayChannel(-1, glockSound, 0);
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
        }
        shot.time_left -= deltaTime; 

        float radians = shot.angle * M_PI / 180.0 ;
        float dx = std::cos(radians);
        float dy = std::sin(radians);


        float prev_x = shot.x;
        float prev_y = shot.y;

        shot.x += dx * SHOT_SPEED * deltaTime;
        shot.y += dy * SHOT_SPEED * deltaTime;

        float dot = (shot.target_x - prev_x) * (shot.target_x - shot.x) + (shot.target_y - prev_y) * (shot.target_y - shot.y);

        if (dot < 0.0f) {
            it = shot_effects.erase(it);
            continue;
        }

        int32_t distance_to_target = BLOCK_SIZE * std::sqrt((shot.x - shot.target_x) * (shot.x - shot.target_x) + (shot.y - shot.target_y) * (shot.y - shot.target_y));
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
            
        ++it;
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
    renderer.SetDrawColor(255, 255, 255, 0);
    renderer.FillRect(interfaceLayout.container);
    Entity player = game.getPlayerState(playerName);
    PlayerData playerData = std::get<PlayerData>(player.data);


    Surface healthLabel = font.RenderText_Blended("Health: " + std::to_string(playerData.health), Color(255, 255, 255));
    Texture healthLabelTexture(renderer, healthLabel);
    Rect healthLabelRect = interfaceLayout.health(interfaceLayout.container, healthLabel, {}, 0);
    renderer.Copy(
        healthLabelTexture,  
        NullOpt, 
        healthLabelRect);


    switch (playerData.equippedWeapon) {
        case WeaponType::PRIMARY: {
            Surface ammoLabel = font.RenderText_Blended("Ammo: " + std::to_string(playerData.inventory.bulletsPrimary), Color(255, 255, 255));
            Texture ammoLabelTexture(renderer, ammoLabel);
            renderer.Copy(
                ammoLabelTexture,  
                NullOpt, 
                interfaceLayout.ammo(interfaceLayout.container, ammoLabel, {healthLabelRect}, 1));
            break;
        }
        case WeaponType::SECONDARY: {
            Surface ammoLabel = font.RenderText_Blended("Ammo: " + std::to_string(playerData.inventory.bulletsSecondary), Color(255, 255, 255));
            Texture ammoLabelTexture(renderer, ammoLabel);
            renderer.Copy(
                ammoLabelTexture,  
                NullOpt, 
                interfaceLayout.ammo(interfaceLayout.container, ammoLabel, {healthLabelRect}, 1));
            break;
        }
        case WeaponType::KNIFE: {
            break;
        }
    }
    Surface timeLabel = font.RenderText_Blended("1:45", Color(255, 255, 255));
    Texture timeLabelTexture(renderer, timeLabel);
    renderer.Copy(
        timeLabelTexture,  
        NullOpt, 
        interfaceLayout.time(interfaceLayout.container, timeLabel));
    std::vector<Rect> equipamiento;
    Rect primaryWeaponContainer;
    Rect primaryWeaponSprite;
    if (playerData.inventory.primary != NONE) { // tiene la primaria
        Rect primaryWeaponContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, 0);
        equipamiento.push_back(primaryWeaponContainer);
        Rect primaryWeaponSprite;
        Texture primaryTexture(renderer, AKInvSprite);
        switch (playerData.inventory.primary) {
            case AK47: {
                primaryWeaponSprite = interfaceLayout.createWeaponSprite(primaryWeaponContainer, AKInvSprite);
                break;
            }
            case M3: {
                primaryWeaponSprite = interfaceLayout.createWeaponSprite(primaryWeaponContainer, M3InvSprite);
                primaryTexture = Texture(renderer, M3InvSprite);
                break;
            }
            case AWP: {
                primaryWeaponSprite = interfaceLayout.createWeaponSprite(primaryWeaponContainer, AWPInvSprite);
                primaryTexture = Texture(renderer, AWPInvSprite);
                break;
            }
            default: {
                break;
            }
        }
        if (playerData.equippedWeapon == WeaponType::PRIMARY) { // tiene la primaria seleccionada
            renderer.SetDrawColor(255, 255, 255, 64);
        } else {
            renderer.SetDrawColor(0, 0, 0, 64);
        }
        renderer.FillRect(primaryWeaponContainer);
        renderer.Copy(
            primaryTexture,
            NullOpt,
            primaryWeaponSprite);
    }
    Rect secondaryWeaponContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, 1);
    Rect secondaryWeaponSprite = interfaceLayout.createWeaponSprite(secondaryWeaponContainer, glockInvSprite);
    equipamiento.push_back(secondaryWeaponContainer);
    if (playerData.equippedWeapon == WeaponType::SECONDARY) { // tiene la secundaria seleccionada
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(0, 0, 0, 64);
    }
    renderer.FillRect(secondaryWeaponContainer);
    Texture secondaryTexture(renderer, glockInvSprite);
    renderer.Copy(
        secondaryTexture,
        NullOpt,
        secondaryWeaponSprite);
    Rect knifeContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, 2);
    Rect knifeSprite = interfaceLayout.createWeaponSprite(knifeContainer, knifeInvSprite);
    equipamiento.push_back(knifeContainer);
    if (playerData.equippedWeapon == WeaponType::KNIFE) { // tiene el cuchillo seleccionado
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(0, 0, 0, 64);
    }
    renderer.FillRect(knifeContainer);
    Texture knifeTexture(renderer, knifeInvSprite);
    renderer.Copy(
        knifeTexture,
        NullOpt,
        knifeSprite);
    
        
    if (playerData.inventory.has_the_bomb) {  // tiene la bomba
        Rect bombContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, 3);
        Rect bombSprite = interfaceLayout.createWeaponSprite(bombContainer, bombInvSprite);
        equipamiento.push_back(bombContainer);
        Texture bombTexture(renderer, bombInvSprite);
        renderer.Copy(
            bombTexture,
            NullOpt,
            bombSprite);
    }

}

InterfaceLayout GameView::createInterfaceLayout() {
    InterfaceLayout layout;

    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();
    const int MARGIN = 15;
    const int CONTAINER_MARGIN = 0;
    const int WEAPON_CONTAINER_HEIGHT = 60;
    const int WEAPON_CONTAINER_WIDTH = 100;
    const int WEAPON_CONTAINER_MARGIN = 5;
    const int HEALTH_AMMO_VERTICAL_SPACING = 5;
    const int INVENTORY_VERTICAL_SPACING = 10;


    layout.container = Rect(
        MARGIN,
        MARGIN,
        width - MARGIN * 2,
        height - MARGIN * 2
    );

    layout.health = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position) {
        // parent = container
        // position = 0;
        // parentsChildren = {}
        uint32_t relative_position = 0;

        for (uint32_t i = 0; i < position; i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - label.GetHeight() - CONTAINER_MARGIN - relative_position,
            label.GetWidth(),
            label.GetHeight()
        );
    };

    layout.ammo = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position) {
        // position = 1;
        // parentsChildren = {layout.health}
        uint32_t relative_position = 0;

        for (uint32_t i = 0; i < position; i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - label.GetHeight() - CONTAINER_MARGIN - relative_position,
            label.GetWidth(),
            label.GetHeight()
        );
    };

    layout.time = [=](Rect parent, Surface& label) {
        return Rect(
            parent.GetX() + parent.GetW()/2,
            parent.GetY() + parent.GetH() - label.GetHeight() - CONTAINER_MARGIN,
            label.GetWidth(),
            label.GetHeight()
        );
    };


    layout.createWeaponContainer = [=](Rect parent, std::vector<Rect> parentsChildren, uint32_t position) {
        // position = 0;
        // parentsChildren = {}
        uint32_t relative_position = 0;

        for (uint32_t i = 0; i < position; i++) {
            relative_position += parentsChildren[i].GetH() + INVENTORY_VERTICAL_SPACING;
        }


        return Rect(
            parent.GetX() + parent.GetW() - WEAPON_CONTAINER_WIDTH - CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - WEAPON_CONTAINER_HEIGHT - CONTAINER_MARGIN - relative_position,
            WEAPON_CONTAINER_WIDTH,
            WEAPON_CONTAINER_HEIGHT
        );
    };

    layout.createWeaponSprite = [=](Rect parent, Surface& sprite) {

        uint32_t weapon_image_container_x = parent.GetX() + WEAPON_CONTAINER_MARGIN;
        uint32_t weapon_image_container_y = parent.GetY() + WEAPON_CONTAINER_MARGIN;
        uint32_t weapon_image_container_w = parent.GetW() - WEAPON_CONTAINER_MARGIN * 2;
        uint32_t weapon_image_container_h = parent.GetH() - WEAPON_CONTAINER_MARGIN * 2;


        float width_to_height_texture_scalar = static_cast<float>(sprite.GetHeight()) / static_cast<float>(sprite.GetWidth());
        float height_to_width_texture_scalar = static_cast<float>(sprite.GetWidth()) / static_cast<float>(sprite.GetHeight());


        uint32_t weapon_texture_x;
        uint32_t weapon_texture_y;
        uint32_t weapon_texture_w;
        uint32_t weapon_texture_h;
        if (
            weapon_image_container_w * width_to_height_texture_scalar 
            > weapon_image_container_h) {
                weapon_texture_y = weapon_image_container_y;
                weapon_texture_h = weapon_image_container_h;
                weapon_texture_w = weapon_image_container_h * height_to_width_texture_scalar;
                weapon_texture_x = weapon_image_container_x + ((weapon_image_container_w - weapon_texture_w)/2);

        } else if (
            weapon_image_container_h * height_to_width_texture_scalar
            > weapon_image_container_w) {
                weapon_texture_x = weapon_image_container_x;
                weapon_texture_w = weapon_image_container_w;
                weapon_texture_h = weapon_image_container_w * width_to_height_texture_scalar;

                weapon_texture_y = weapon_image_container_y;
                weapon_texture_y = weapon_image_container_y + ((weapon_image_container_h - weapon_texture_h)/2);

        }

        return Rect(
            weapon_texture_x, 
            weapon_texture_y, 
            weapon_texture_w, 
            weapon_texture_h);
    };


    return layout;
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
    uint32_t money = std::get<PlayerData>(player.data).money;
    Inventory inv = std::get<PlayerData>(player.data).inventory;
    
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

    std::string ammoBoughtText;
    if (inv.primary) {
        ammoBoughtText = std::to_string(inv.bulletsSecondary) + "/" + std::to_string(Weapons::getWeapon(inv.secondary).maxAmmo);
    } else {
        ammoBoughtText = "No disponible";
    }
    Surface secondaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsSecondary) + "/" + std::to_string(Weapons::getWeapon(inv.secondary).maxAmmo), Color(255, 255, 255));
    Texture secondaryAmmoBoughtLabelTexture(renderer, secondaryAmmoBoughtLabel);

    Surface secondaryAmmoLabel = font.RenderText_Blended("Second.", Color(255, 255, 255));
    Texture secondaryAmmoLabelTexture(renderer, secondaryAmmoLabel);
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    Texture secondaryAmmoPriceLabelTexture(renderer, secondaryAmmoPriceLabel);
    renderer.Copy(secondaryAmmoLabelTexture, NullOpt, shopLayout.secondaryAmmoLabel); 
    renderer.Copy(secondaryAmmoPriceLabelTexture, NullOpt, shopLayout.secondaryAmmoPriceLabel); 

    renderer.Copy(
        secondaryAmmoBoughtLabelTexture, 
        NullOpt, 
        Rect(
            shopLayout.secondaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN + ((shopLayout.secondaryAmmoContainer.GetW() - secondaryAmmoBoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), 
            shopLayout.secondaryAmmoContainer.GetY() + shopLayout.secondaryAmmoContainer.GetH() - secondaryAmmoBoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, 
            secondaryAmmoBoughtLabelTexture.GetWidth(), 
            secondaryAmmoBoughtLabelTexture.GetHeight()));


    const int CONTAINER_MARGIN = 20;
    Surface moneyLabel = font.RenderText_Blended("Money: $" + std::to_string(money), Color(255, 255, 255));
    Texture moneyLabelTexture = Texture(renderer, moneyLabel);
    Rect moneyLabelRect(
        shopLayout.container.GetX() + shopLayout.container.GetW() - CONTAINER_MARGIN - moneyLabel.GetWidth(), 
        shopLayout.container.GetY() + shopLayout.container.GetH() - CONTAINER_MARGIN - moneyLabel.GetHeight(), 
        moneyLabel.GetWidth(), 
        moneyLabel.GetHeight());

    renderer.Copy(
        moneyLabelTexture, 
        NullOpt, 
        moneyLabelRect);
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


    // Delirio
    /////////////////////////////////////////////////////////////////////////
    /*
    SdlNullWidget nullWidget;
    SdlContainer container(
        Rect(
            0, 0, width, height
        ),
        nullWidget
    );
    container.setMargin(CONTAINER_MARGIN);
    container.setPadding(MARGIN);

    SdlVBoxContainer sectionsContainer;
    sectionsContainer.setSpacing(CONTAINER_VERTICAL_SPACING);

    SdlHBoxContainer weaponSectionVBox;
    weaponSectionVBox.setSpacing(SECTION_VERTICAL_SPACING);
    SdlHBoxContainer ammoSectionVBox;
    ammoSectionVBox.setSpacing(SECTION_VERTICAL_SPACING);

    SdlHBoxContainer weaponContainersHBox; 
    weaponContainersHBox.setSpacing(SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING);


    Surface primaryWeaponSectionLabel = font.RenderText_Blended("PRIMARY WEAPONS", Color(255, 255, 255));
    SdlSurfaceWidget primaryWeaponSectionLabelWidget(primaryWeaponSectionLabel);
    primaryWeaponSectionLabelWidget.setSizePolicy(FIXED);

    SdlHBoxContainer ammoContainersHBox;
    ammoContainersHBox.setSpacing(SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING);

    Surface ammoSectionLabel = font.RenderText_Blended("AMMO", Color(255, 255, 255));
    SdlSurfaceWidget ammoSectionLabelWidget(ammoSectionLabel);
    ammoSectionLabelWidget.setSizePolicy(FIXED);


    auto shop = game.getStore();

    for (size_t i = 0; i < shop.size(); i++) {
        WeaponName weapon = shop[i].first;
        int price = shop[i].second;
        std::string weaponLabelText;

        switch (weapon) {
            case WeaponName::AK47: {
                weaponLabelText = "AK-47";
                break;
            }
            case WeaponName::AWP: {
                weaponLabelText = "AWP";
                break;
            }
            case WeaponName::M3: {
                weaponLabelText = "M3";
                break;
            }
            default: {
                break;
            }
        }

        SdlContainer weaponItemContainer(
            Rect(),
            nullWidget
        );
        weaponItemContainer.setMargin(ITEM_CONTAINER_MARGIN);

        SdlVBoxContainer weaponInfoContainer;

        weaponInfoContainer.setSpacing(ITEM_CONTAINER_VERTICAL_SPACING);

        Surface weaponLabel = font.RenderText_Blended(weaponLabelText, Color(255, 255, 255));
        Surface priceLabel = font.RenderText_Blended(std::to_string(price), Color(255, 255, 255));
        SdlHBoxContainer weaponAndPriceVBox;
        Surface weaponBoughtLabel = font.RenderText_Blended(std::to_string(price), Color(255, 255, 255));
        SdlSurfaceWidget weaponLabelWidget(weaponLabel);
        weaponLabelWidget.setSizePolicy(FIXED);
        SdlSurfaceWidget weaponBoughtLabelWidget(weaponBoughtLabel);
        weaponBoughtLabelWidget.setSizePolicy(FIXED);
        SdlSurfaceWidget weaponPriceLabelWidget(priceLabel);
        weaponPriceLabelWidget.setSizePolicy(FIXED);
        SdlSurfaceWidget weaponSpriteWidget((weapon == AK47 ? AK47ShopSprite : (weapon == M3 ? M3ShopSprite : AWPShopSprite))); // solucion fea, podria hacer q el widget reciba un path directamente
        weaponSpriteWidget.setSizePolicy(EXPAND_BUT_MAINTAIN_RATIO);

        weaponAndPriceVBox.addChild(weaponLabelWidget);
        weaponAndPriceVBox.addChild(weaponPriceLabelWidget);
        weaponInfoContainer.addChild(weaponAndPriceVBox);
        weaponInfoContainer.addChild(weaponBoughtLabelWidget);

        weaponItemContainer.setChild(Alignment{MIDDLE, CENTER}, weaponInfoContainer);
    }

    
    SdlContainer pAmmoItemContainer(
        Rect(),
        nullWidget
    );

    pAmmoItemContainer.setMargin(ITEM_CONTAINER_MARGIN);

    SdlVBoxContainer pAmmoInfoContainer;

    pAmmoInfoContainer.setSpacing(ITEM_CONTAINER_VERTICAL_SPACING);

    Surface pAmmoLabel = font.RenderText_Blended("Primary", Color(255, 255, 255));
    Surface pAmmoPriceLabel = font.RenderText_Blended(std::to_string(AMMO_PRICE), Color(255, 255, 255));
    SdlHBoxContainer pAmmoAndPriceVBox;
    Surface pAmmoBoughtLabel = font.RenderText_Blended("amount", Color(255, 255, 255));
    SdlSurfaceWidget pAmmoLabelWidget(pAmmoLabel);
    pAmmoLabelWidget.setSizePolicy(FIXED);
    SdlSurfaceWidget pAmmoBoughtLabelWidget(pAmmoBoughtLabel);
    pAmmoBoughtLabelWidget.setSizePolicy(FIXED);
    SdlSurfaceWidget pAmmoPriceLabelWidget(pAmmoPriceLabel);
    pAmmoPriceLabelWidget.setSizePolicy(FIXED);
    
    pAmmoAndPriceVBox.addChild(pAmmoLabelWidget);
    pAmmoAndPriceVBox.addChild(pAmmoPriceLabelWidget);
    pAmmoInfoContainer.addChild(pAmmoAndPriceVBox);
    pAmmoInfoContainer.addChild(pAmmoBoughtLabelWidget);

    pAmmoItemContainer.setChild(Alignment{MIDDLE, CENTER}, pAmmoInfoContainer);

    



    SdlContainer sAmmoItemContainer(
        Rect(),
        nullWidget
    );

    SdlVBoxContainer sAmmoInfoContainer;

    sAmmoItemContainer.setMargin(ITEM_CONTAINER_MARGIN);

    sAmmoInfoContainer.setSpacing(ITEM_CONTAINER_VERTICAL_SPACING);

    Surface sAmmoLabel = font.RenderText_Blended("Secondary", Color(255, 255, 255));
    Surface sAmmoPriceLabel = font.RenderText_Blended(std::to_string(AMMO_PRICE), Color(255, 255, 255));
    SdlHBoxContainer sAmmoAndPriceVBox;
    Surface sAmmoBoughtLabel = font.RenderText_Blended("amount", Color(255, 255, 255));
    SdlSurfaceWidget sAmmoLabelWidget(sAmmoLabel);
    sAmmoLabelWidget.setSizePolicy(FIXED);
    SdlSurfaceWidget sAmmoBoughtLabelWidget(sAmmoBoughtLabel);
    sAmmoBoughtLabelWidget.setSizePolicy(FIXED);
    SdlSurfaceWidget sAmmoPriceLabelWidget(sAmmoPriceLabel);
    sAmmoPriceLabelWidget.setSizePolicy(FIXED);
    
    sAmmoAndPriceVBox.addChild(sAmmoLabelWidget);
    sAmmoAndPriceVBox.addChild(sAmmoPriceLabelWidget);
    sAmmoInfoContainer.addChild(sAmmoAndPriceVBox);
    sAmmoInfoContainer.addChild(sAmmoBoughtLabelWidget);

    sAmmoItemContainer.setChild(Alignment{MIDDLE, CENTER}, sAmmoInfoContainer);

    weaponSectionVBox.addChild(primaryWeaponSectionLabelWidget);
    weaponSectionVBox.addChild(weaponContainersHBox);

    ammoSectionVBox.addChild(ammoSectionLabelWidget);
    ammoSectionVBox.addChild(ammoContainersHBox);

    sectionsContainer.addChild(weaponSectionVBox);
    sectionsContainer.addChild(ammoSectionVBox);


    container.setChild(Alignment{MIDDLE, CENTER}, sectionsContainer);

    */
    /////////////////////////////////////////////////

    
    Rect container(MARGIN, MARGIN, width - 2 * MARGIN, height - 2 * MARGIN);
    layout.container = container;
    
    int item_container_hbox_width = container.GetW() - CONTAINER_MARGIN * 2;


    auto shop = game.getStore();


    
    Surface primaryWeaponSectionLabel = font.RenderText_Blended("PRIMARY WEAPONS", Color(255, 255, 255));

    Rect primaryWeaponSectionLabelRect(
            container.GetX() + CONTAINER_MARGIN, 
            container.GetY() + CONTAINER_MARGIN, 
            primaryWeaponSectionLabel.GetWidth(), 
            primaryWeaponSectionLabel.GetHeight());
    
    layout.primaryWeaponLabel = primaryWeaponSectionLabelRect;


    Surface BoughtLabel = font.RenderText_Blended("Comprada", Color(255, 255, 255));
    
    int item_container_x = container.GetX() + CONTAINER_MARGIN;
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

        Surface weaponPriceLabel = font.RenderText_Blended("$" + std::to_string(price), Color(255, 255, 255));
        Rect weaponPriceLabelRect(
                item_container.GetX() + item_container.GetW() - weaponPriceLabel.GetWidth() - ITEM_CONTAINER_MARGIN, 
                item_container.GetY() + ITEM_CONTAINER_MARGIN, 
                weaponPriceLabel.GetWidth(), 
                weaponPriceLabel.GetHeight());
        layout.weaponPriceLabels.push_back(weaponPriceLabelRect);
        
        uint32_t weapon_image_container_x = item_container.GetX() + ITEM_CONTAINER_MARGIN;
        uint32_t weapon_image_container_y = item_container.GetY() + ITEM_CONTAINER_MARGIN + weaponLabel.GetHeight() + ITEM_CONTAINER_VERTICAL_SPACING;
        uint32_t weapon_image_container_w = item_container.GetW() - ITEM_CONTAINER_MARGIN * 2;
        uint32_t weapon_image_container_h = item_container.GetH() - ITEM_CONTAINER_MARGIN * 2 - ITEM_CONTAINER_VERTICAL_SPACING * 2 - weaponLabel.GetHeight() * 2;


        float width_to_height_texture_scalar = static_cast<float>(weaponTexture.GetHeight()) / static_cast<float>(weaponTexture.GetWidth());
        float height_to_width_texture_scalar = static_cast<float>(weaponTexture.GetWidth()) / static_cast<float>(weaponTexture.GetHeight());


        uint32_t weapon_texture_x;
        uint32_t weapon_texture_y;
        uint32_t weapon_texture_w;
        uint32_t weapon_texture_h;
        if (
            weapon_image_container_w * width_to_height_texture_scalar 
            > weapon_image_container_h) {
                weapon_texture_y = weapon_image_container_y;
                weapon_texture_h = weapon_image_container_h;
                weapon_texture_w = weapon_image_container_h * height_to_width_texture_scalar;
                weapon_texture_x = weapon_image_container_x + ((weapon_image_container_w - weapon_texture_w)/2);

        } else if (
            weapon_image_container_h * height_to_width_texture_scalar
            > weapon_image_container_w) {
                weapon_texture_x = weapon_image_container_x;
                weapon_texture_w = weapon_image_container_w;
                weapon_texture_h = weapon_image_container_w * width_to_height_texture_scalar;

                weapon_texture_y = weapon_image_container_y;
                weapon_texture_y = weapon_image_container_y + ((weapon_image_container_h - weapon_texture_h)/2);

        }

        Rect weaponTextureRect(
            weapon_texture_x, 
            weapon_texture_y, 
            weapon_texture_w, 
            weapon_texture_h);
        layout.weaponSprites.push_back(weaponTextureRect);


        Rect boughtLabelRect(
            item_container.GetX() + ITEM_CONTAINER_MARGIN + ((item_container.GetW() - BoughtLabel.GetWidth() - 2 * ITEM_CONTAINER_MARGIN)/2), 
            item_container.GetY() + item_container.GetH() - BoughtLabel.GetHeight() - ITEM_CONTAINER_MARGIN, 
            BoughtLabel.GetWidth(), 
            BoughtLabel.GetHeight());
        layout.boughtLabels.push_back(boughtLabelRect);
        
    }


    Surface ammoSectionLabel = font.RenderText_Blended("AMMO", Color(255, 255, 255));
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

    Surface primaryAmmoLabel = font.RenderText_Blended("Primary", Color(255, 255, 255));
    Surface primaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
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

    Surface secondaryAmmoLabel = font.RenderText_Blended("Second.", Color(255, 255, 255));
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));

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