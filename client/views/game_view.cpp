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

        akShopSprite.SetColorKey(true, SDL_MapRGB(akShopSprite.Get()->format, 255, 0, 255));
        m3ShopSprite.SetColorKey(true, SDL_MapRGB(m3ShopSprite.Get()->format, 255, 0, 255));
        awpShopSprite.SetColorKey(true, SDL_MapRGB(awpShopSprite.Get()->format, 255, 0, 255));

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
    // TODO: cambiar el tamaño de las cosas cuando se cambia el tamaño de la ventana
    // int block_size = renderer.GetOutputHeight() / 24;
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
// TODO: Fijarse en el TODO de createInterfaceLayout
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
    int position = 0;
    if (playerData.inventory.primary != WeaponName::NONE) { // tiene la primaria

        Rect primaryWeaponContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, position++);
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

    Rect secondaryWeaponContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, position++);
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
    Rect knifeContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, position++);
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
        Rect bombContainer = interfaceLayout.createWeaponContainer(interfaceLayout.container, equipamiento, position++);
        Rect bombSprite = interfaceLayout.createWeaponSprite(bombContainer, bombInvSprite);
        equipamiento.push_back(bombContainer);
        Texture bombTexture(renderer, bombInvSprite);
        renderer.Copy(
            bombTexture,
            NullOpt,
            bombSprite);
    }

}


// TODO: Plantear de otra forma para que los elementos se puedan acceder de manera fija, no en forma de funciones
// Y que esto se vuelva a ejecutar cada vez q el jugador hace algo para que cambie
InterfaceLayout GameView::createInterfaceLayout() {
    InterfaceLayout layout;

    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();
    const int MARGIN = 15;
    const int CONTAINER_MARGIN = 0;
    const int WEAPON_CONTAINER_HEIGHT = 60.0f * static_cast<float>(height)/480.0f;
    const int WEAPON_CONTAINER_WIDTH = 100.0f * static_cast<float>(width)/640.0f;
    const int WEAPON_CONTAINER_MARGIN = 5;
    const int HEALTH_AMMO_VERTICAL_SPACING = 5;
    const int INVENTORY_VERTICAL_SPACING = 10;
    float text_scale = 1.0f/static_cast<float>(FONT_SIZE) * (static_cast<float>(height)/24.0f);


    layout.container = Rect(
        MARGIN,
        MARGIN,
        width - MARGIN * 2,
        height - MARGIN * 2
    );

    layout.health = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position) {
        uint32_t relative_position = 0;

        for (uint32_t i = 0; i < position; i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - label.GetHeight() * text_scale - CONTAINER_MARGIN - relative_position,
            label.GetWidth() * text_scale,
            label.GetHeight() * text_scale 
        );
    };

    layout.ammo = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren, uint32_t position) {
        uint32_t relative_position = 0;

        for (uint32_t i = 0; i < position; i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - label.GetHeight() * text_scale - CONTAINER_MARGIN - relative_position,
            label.GetWidth() * text_scale,
            label.GetHeight() * text_scale
        );
    };

    layout.time = [=](Rect parent, Surface& label) {
        return Rect(
            parent.GetX() + parent.GetW()/2 - label.GetHeight() * text_scale / 2,
            parent.GetY() + parent.GetH() - label.GetHeight() * text_scale - CONTAINER_MARGIN,
            label.GetWidth() * text_scale,
            label.GetHeight() * text_scale
        );
    };


    layout.createWeaponContainer = [=](Rect parent, std::vector<Rect> parentsChildren, uint32_t position) {
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

void GameView::resizeHud() {
    interfaceLayout = createInterfaceLayout();
}

// TODO: Metodo extremadamente largo, cambiar el planteamiento para que por un lado se creen los Rect
// y se almacenen de algun modo, y cada vez que se actualiza algo (osea, el jugador compra algo o cambia el tamaño de la ventana)
// se vuelvan a crear los Rects. Por otro lado va a ver un metodo que grafique los sprites, los textos,
// y los contenedores de la interfaz dentro de esos rects.
void GameView::showShop() {
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();
    const int MARGIN = 40;
    const int CONTAINER_MARGIN = 20;
    const int SECTION_VERTICAL_SPACING = 10; // el spacing que hay entre el label Primary Weapon y los botones de las armas, mismo para la seccion de ammo
    const int SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING = 20;
    const int CONTAINER_VERTICAL_SPACING = 20;
    const int ITEM_CONTAINER_MARGIN = 10;
    const int ITEM_CONTAINER_VERTICAL_SPACING = 5;

    float text_scale = 1.0f/static_cast<float>(FONT_SIZE) * (static_cast<float>(height)/24.0f);



    //////////////////////////////////////////////////////////////////////
    Rect container(
        MARGIN, 
        MARGIN, 
        width - 2 * MARGIN, 
        height - 2 * MARGIN);
    //////////////////////////////////////////////////////////////////////

    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(container);
    
    int item_container_hbox_width = container.GetW() - CONTAINER_MARGIN * 2;


    auto shop = game.getStore();


    
    Surface primaryWeaponSectionLabel = font.RenderText_Blended("PRIMARY WEAPONS", Color(255, 255, 255));
    Texture primaryWeaponSectionLabelTexture(renderer, primaryWeaponSectionLabel);

    ///////////////////////////////////////////////////
    Rect primaryWeaponSectionLabelRect(
            container.GetX() + CONTAINER_MARGIN,
            container.GetY() + CONTAINER_MARGIN, 
            primaryWeaponSectionLabel.GetWidth() * text_scale, 
            primaryWeaponSectionLabel.GetHeight() * text_scale);
    ///////////////////////////////////////////////////

    renderer.Copy(
        primaryWeaponSectionLabelTexture, 
        NullOpt, 
        primaryWeaponSectionLabelRect);



    Surface BoughtLabel = font.RenderText_Blended("Comprada", Color(255, 255, 255));
    Texture BoughtLabelTexture(renderer, BoughtLabel);


    PlayerData player = std::get<PlayerData>(game.getPlayerState(playerName).data);
    uint32_t money = player.money;
    Inventory inv = player.inventory;
    
    
    int itemContainerX = container.GetX() + CONTAINER_MARGIN;
    for (size_t i = 0; i < shop.size(); i++) {
        WeaponName weapon = shop[i].first;
        int price = shop[i].second;
        std::string weaponLabelText = weaponTexts[weapon];

        ////////////////////////////////////////////////////
        Rect itemContainer(
            itemContainerX, 
            container.GetY() + CONTAINER_MARGIN + primaryWeaponSectionLabelRect.GetH() + SECTION_VERTICAL_SPACING, 
            (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.size() - 1))/shop.size(), 
            container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabelRect.GetH() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
        //////////////////////////////////////////////////
        weaponShopButtons[weapon] = {{itemContainer.GetX(), itemContainer.GetY()}, {itemContainer.GetW(), itemContainer.GetH()}};


        if (inv.primary == weapon) {
            renderer.SetDrawColor(255, 255, 255, 64);
        } else {
            renderer.SetDrawColor(255, 255, 255, 32);
        }
        renderer.FillRect(itemContainer);


        ///////////////////////////////////////////////
        Surface weaponLabel = font.RenderText_Blended(weaponLabelText, Color(255, 255, 255));
        Rect weaponLabelRect(
            itemContainer.GetX() + ITEM_CONTAINER_MARGIN, 
            itemContainer.GetY() + ITEM_CONTAINER_MARGIN, 
            weaponLabel.GetWidth() * text_scale, 
            weaponLabel.GetHeight() * text_scale);
        /////////////////////////////////////////////

        Texture weaponLabelTexture(renderer, weaponLabel);
        renderer.Copy(
            weaponLabelTexture, 
            NullOpt, 
            weaponLabelRect);

        ////////////////////////////////////////////////
        Surface weaponPriceLabel = font.RenderText_Blended("$" + std::to_string(price), Color(255, 255, 255));
        Rect weaponPriceLabelRect(
            itemContainer.GetX() + itemContainer.GetW() - weaponPriceLabel.GetWidth() * text_scale - ITEM_CONTAINER_MARGIN, 
            itemContainer.GetY() + ITEM_CONTAINER_MARGIN, 
            weaponPriceLabel.GetWidth() * text_scale, 
            weaponPriceLabel.GetHeight() * text_scale);
        ///////////////////////////////////////////////

        Texture weaponPriceLabelTexture(renderer, weaponPriceLabel);
        renderer.Copy(
            weaponPriceLabelTexture, 
            NullOpt, 
            weaponPriceLabelRect);


        


        /////////////////////////////////////////////////////////////////////////////////////////
        Surface& weaponSprite = getWeaponSprite(weapon);


        uint32_t weapon_image_container_x = itemContainer.GetX() + ITEM_CONTAINER_MARGIN;
        uint32_t weapon_image_container_y = itemContainer.GetY() + ITEM_CONTAINER_MARGIN + weaponLabelRect.GetH() + ITEM_CONTAINER_VERTICAL_SPACING;
        uint32_t weapon_image_container_w = itemContainer.GetW() - ITEM_CONTAINER_MARGIN * 2;
        uint32_t weapon_image_container_h = itemContainer.GetH() - ITEM_CONTAINER_MARGIN * 2 - ITEM_CONTAINER_VERTICAL_SPACING * 2 - weaponLabelRect.GetH() * 2;



        float height_to_width_texture_ratio = static_cast<float>(weaponSprite.GetWidth()) / static_cast<float>(weaponSprite.GetHeight());


        uint32_t weapon_texture_x;
        uint32_t weapon_texture_y;
        uint32_t weapon_texture_w;
        uint32_t weapon_texture_h;
        if (
            weapon_image_container_w / height_to_width_texture_ratio
            > weapon_image_container_h) {
                weapon_texture_y = weapon_image_container_y;
                weapon_texture_h = weapon_image_container_h;
                weapon_texture_w = weapon_image_container_h * height_to_width_texture_ratio;
                weapon_texture_x = weapon_image_container_x + ((weapon_image_container_w - weapon_texture_w)/2);

        } else if (
            weapon_image_container_h * height_to_width_texture_ratio
            > weapon_image_container_w) {
                weapon_texture_x = weapon_image_container_x;
                weapon_texture_w = weapon_image_container_w;
                weapon_texture_h = weapon_image_container_w / height_to_width_texture_ratio;

                weapon_texture_y = weapon_image_container_y;
                weapon_texture_y = weapon_image_container_y + ((weapon_image_container_h - weapon_texture_h)/2);

        }

        Rect weaponTextureRect(
            weapon_texture_x, 
            weapon_texture_y, 
            weapon_texture_w, 
            weapon_texture_h);
        /////////////////////////////////////////////////////////////////////////////////////
        Texture weaponTexture(renderer, weaponSprite);
        renderer.Copy(
            weaponTexture, 
            NullOpt, 
            weaponTextureRect);


        //////////////////////////////////////////////////////////////////////////////////
        Rect boughtLabelRect(
            itemContainer.GetX() + ITEM_CONTAINER_MARGIN + ((itemContainer.GetW() - BoughtLabel.GetWidth() * text_scale - 2 * ITEM_CONTAINER_MARGIN)/2), 
            itemContainer.GetY() + itemContainer.GetH() - BoughtLabel.GetHeight() * text_scale - ITEM_CONTAINER_MARGIN, 
            BoughtLabel.GetWidth() * text_scale, 
            BoughtLabel.GetHeight() * text_scale);
        ///////////////////////////////////////////////////////////////////////////////////
        if (inv.primary == weapon) {
            renderer.Copy(
                BoughtLabelTexture, 
                NullOpt, 
                boughtLabelRect);
        }

        itemContainerX = itemContainer.GetX() + itemContainer.GetW() + SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING;
    }

    ///////////////////////////////////////////////////////////////////////////////
    Surface ammoSectionLabel = font.RenderText_Blended("AMMO", Color(255, 255, 255));
    Rect ammoSectionLabelRect(
        container.GetX() + CONTAINER_MARGIN, 
        container.GetY() + container.GetH() / 2 + CONTAINER_VERTICAL_SPACING / 2, 
        ammoSectionLabel.GetWidth() * text_scale, 
        ammoSectionLabel.GetHeight() * text_scale);
    ////////////////////////////////////////////////////////////////////////////////

    Texture ammoSectionLabelTexture(renderer, ammoSectionLabel);
    renderer.Copy(ammoSectionLabelTexture, NullOpt, ammoSectionLabelRect);


    ///////////////////////////////////////////////////////////////////////////////////
    Rect primaryAmmoContainer(
        container.GetX() + CONTAINER_MARGIN, 
        container.GetY() + container.GetH() / 2 + CONTAINER_VERTICAL_SPACING / 2 + ammoSectionLabelRect.GetH() + SECTION_VERTICAL_SPACING, 
        (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.size() - 1))/3, 
        container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabelRect.GetH() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    buyPrimaryAmmoButton = {{primaryAmmoContainer.GetX(), primaryAmmoContainer.GetY()}, {primaryAmmoContainer.GetW(), primaryAmmoContainer.GetH()}};

    if (inv.bulletsPrimary >= Weapons::getWeapon(inv.primary).maxAmmo && inv.primary != WeaponName::NONE) {
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(255, 255, 255, 32);
    }
    renderer.FillRect(primaryAmmoContainer);

    ///////////////////////////////////////////////////////////////
    Rect secondaryAmmoContainer(
        primaryAmmoContainer.GetX() + primaryAmmoContainer.GetW() + SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING,
        primaryAmmoContainer.GetY(), 
        primaryAmmoContainer.GetW(), 
        primaryAmmoContainer.GetH());
    //////////////////////////////////////////////////////////////////
    buySecondaryAmmoButton = {{secondaryAmmoContainer.GetX(), secondaryAmmoContainer.GetY()}, {secondaryAmmoContainer.GetW(), secondaryAmmoContainer.GetH()}};

    if (inv.bulletsSecondary >= Weapons::getWeapon(inv.secondary).maxAmmo) {
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(255, 255, 255, 32);
    }
    renderer.FillRect(secondaryAmmoContainer);

    Surface primaryAmmoLabel = font.RenderText_Blended("Primary", Color(255, 255, 255));
    Rect primaryAmmoLabelRect(
        primaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoLabel.GetWidth() * text_scale, 
        primaryAmmoLabel.GetHeight() * text_scale);


    Texture primaryAmmoLabelTexture(renderer, primaryAmmoLabel);
    renderer.Copy(primaryAmmoLabelTexture, NullOpt, primaryAmmoLabelRect);


    Surface primaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    Rect primaryAmmoPriceLabelRect(
        primaryAmmoContainer.GetX() + primaryAmmoContainer.GetW() - primaryAmmoPriceLabel.GetWidth() * text_scale - ITEM_CONTAINER_MARGIN, 
        primaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoPriceLabel.GetWidth() * text_scale, 
        primaryAmmoPriceLabel.GetHeight() * text_scale);

    Texture primaryAmmoPriceLabelTexture(renderer, primaryAmmoPriceLabel);
    renderer.Copy(primaryAmmoPriceLabelTexture, NullOpt, primaryAmmoPriceLabelRect);

    ///////////////////////////////////////////////////////////
    std::string ammoBoughtText;
    if (inv.primary != WeaponName::NONE) {
        ammoBoughtText = std::to_string(inv.bulletsPrimary) + "/" + std::to_string(Weapons::getWeapon(inv.primary).maxAmmo);
    } else {
        ammoBoughtText = "No disponible";
    }
    Surface primaryAmmoBoughtLabel = font.RenderText_Blended(ammoBoughtText, Color(255, 255, 255));
    Rect primaryAmmoBoughtLabelRect(
            primaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN + ((primaryAmmoContainer.GetW() - primaryAmmoBoughtLabel.GetWidth() * text_scale - 2 * ITEM_CONTAINER_MARGIN)/2), 
            primaryAmmoContainer.GetY() + primaryAmmoContainer.GetH() - primaryAmmoBoughtLabel.GetHeight() * text_scale - ITEM_CONTAINER_MARGIN, 
            primaryAmmoBoughtLabel.GetWidth() * text_scale, 
            primaryAmmoBoughtLabel.GetHeight() * text_scale);
    //////////////////////////////////////////////////


    Texture primaryAmmoBoughtLabelTexture(renderer, primaryAmmoBoughtLabel);
    renderer.Copy(
        primaryAmmoBoughtLabelTexture, 
        NullOpt, 
        primaryAmmoBoughtLabelRect);



    //////////////////////////////////////////////////////////////////////////////////////////
    Surface secondaryAmmoLabel = font.RenderText_Blended("Second.", Color(255, 255, 255));
    Rect secondaryAmmoLabelRect(
        secondaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoLabel.GetWidth() * text_scale, 
        secondaryAmmoLabel.GetHeight() * text_scale);
    ////////////////////////////////////////////////////////////////////////////////

    Texture secondaryAmmoLabelTexture(renderer, secondaryAmmoLabel);
    renderer.Copy(secondaryAmmoLabelTexture, NullOpt, secondaryAmmoLabelRect); 


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + std::to_string(AMMO_PRICE), Color(255, 255, 255));
    Rect secondaryAmmoPriceLabelRect(
        secondaryAmmoContainer.GetX() + secondaryAmmoContainer.GetW() - secondaryAmmoPriceLabel.GetWidth() * text_scale - ITEM_CONTAINER_MARGIN, 
        secondaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoPriceLabel.GetWidth() * text_scale, 
        secondaryAmmoPriceLabel.GetHeight() * text_scale);
    //////////////////////////////////////////////////////////////////////////////////////////////

    Texture secondaryAmmoPriceLabelTexture(renderer, secondaryAmmoPriceLabel);
    renderer.Copy(secondaryAmmoPriceLabelTexture, NullOpt, secondaryAmmoPriceLabelRect); 

    ///////////////////////////////////////////////////////////

    Surface secondaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsSecondary) + "/" + std::to_string(Weapons::getWeapon(inv.secondary).maxAmmo), Color(255, 255, 255));
    Rect secondaryAmmoBoughtLabelRect(
            secondaryAmmoContainer.GetX() + ITEM_CONTAINER_MARGIN + ((secondaryAmmoContainer.GetW() - secondaryAmmoBoughtLabel.GetWidth() * text_scale - 2 * ITEM_CONTAINER_MARGIN)/2), 
            secondaryAmmoContainer.GetY() + secondaryAmmoContainer.GetH() - secondaryAmmoBoughtLabel.GetHeight() * text_scale - ITEM_CONTAINER_MARGIN, 
            secondaryAmmoBoughtLabel.GetWidth() * text_scale, 
            secondaryAmmoBoughtLabel.GetHeight() * text_scale);
    
    //////////////////////////////////////////////////////////////////////////
    Texture secondaryAmmoBoughtLabelTexture(renderer, secondaryAmmoBoughtLabel);
    renderer.Copy(secondaryAmmoBoughtLabelTexture, NullOpt, secondaryAmmoBoughtLabelRect);


    ///////////////////////////////////////////////////////////////////
    Surface moneyLabel = font.RenderText_Blended("Money: $" + std::to_string(money), Color(255, 255, 255));
    Rect moneyLabelRect(
        container.GetX() + container.GetW() - CONTAINER_MARGIN - moneyLabel.GetWidth() * text_scale, 
        container.GetY() + container.GetH() - CONTAINER_MARGIN - moneyLabel.GetHeight() * text_scale, 
        moneyLabel.GetWidth() * text_scale, 
        moneyLabel.GetHeight() * text_scale);
    ///////////////////////////////////////////////////////////////////////////////

    Texture moneyLabelTexture(renderer, moneyLabel);
    renderer.Copy(moneyLabelTexture, NullOpt, moneyLabelRect);
}


SDL_Point GameView::getCenterPoint() {
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();

    SDL_Point center = { width / 2, height / 2 };
    return center;
}



void GameView::addShotEffect(Bullet shot) {
    shot_effects.push_back(ShotEffect{shot.origin_x, shot.origin_y, shot.target_x, shot.target_y, shot.angle, SHOT_DURATION});
}


void GameView::switchShopVisibility() {
    shopIsVisible = !shopIsVisible;
}