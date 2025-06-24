#include "game_view.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include "../../common/structures.h"

namespace fs = std::filesystem;


GameView::GameView(const std::string& playerName, SDL_Point window_pos, Map& map, std::vector<WeaponInfo>& weapons, Shop& shop, std::vector<PlayerInfo>& players)
    : 
    window(createWindow(window_pos)), 
    renderer(createRenderer(window)),
    playerName(playerName), map(map), weapons(weapons), shop(shop), players(players),
    mapTiles(Texture(renderer, map.get_sprite_path())), 
    backgroundTexture(renderer, map.get_background_path()), 
    bloodTexture(createBloodTexture()), 
    sparkTexture(createSparkTexture()),
    akShopSprite(createShopTexture(BASE_PATH + "gfx/weapons/ak47_m.bmp")),
    m3ShopSprite(createShopTexture(BASE_PATH + "gfx/weapons/m3_m.bmp")),
    awpShopSprite(createShopTexture(BASE_PATH + "gfx/weapons/awp_m.bmp")),
    akInvSprite(createInvTexture(BASE_PATH + "gfx/weapons/ak47_k.bmp")),
    m3InvSprite(createInvTexture(BASE_PATH + "gfx/weapons/m3_k.bmp")),
    awpInvSprite(createInvTexture(BASE_PATH + "gfx/weapons/awp_k.bmp")),
    glockInvSprite(createInvTexture(BASE_PATH + "gfx/weapons/glock_k.bmp")),
    knifeInvSprite(createInvTexture(BASE_PATH + "gfx/weapons/knife_k.bmp")),
    bombInvSprite(createInvTexture(BASE_PATH + "gfx/weapons/bomb_d.bmp")) {
}


Window GameView::createWindow(SDL_Point window_pos) {
	return Window("Counter Strike 2D",
			window_pos.x, window_pos.y,
			640, 480,
			SDL_WINDOW_RESIZABLE);
}

Renderer GameView::createRenderer(Window& window) {
    Renderer r(window, -1, SDL_RENDERER_ACCELERATED);
    r.SetDrawColor(0, 0, 0, 255);
    r.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    return r;
}


void GameView::update(float deltaTime) {
    renderer.Clear();
    phaseTimer -= deltaTime;

    Entity playerEntity;
    PlayerData playerData;
    for (size_t i = 0; i < state.entities.size(); i++) {
        Entity entity = state.entities[i];
        if (entity.type == PLAYER) {
            PlayerData data = std::get<PlayerData>(entity.data);
            if (data.name == playerName) {
                playerData = data;
                playerEntity = entity;
                break;
            }
        }
    }

    // TODO: cambiar el tamaño de las cosas cuando se cambia el tamaño de la ventana
    // int block_size = renderer.GetOutputHeight() / 24;
    SDL_Point center = getCenterPoint();

    float cameraX = center.x - playerEntity.x * BLOCK_SIZE - BLOCK_SIZE / 2 + (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2;
    float cameraY = center.y - playerEntity.y * BLOCK_SIZE - BLOCK_SIZE / 2 + (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2;


    showBackground();
    showMap(cameraX, cameraY);
    showBullets(cameraX, cameraY, deltaTime);
    showBloodEffects(cameraX, cameraY, deltaTime);
    showSparksEffects(cameraX, cameraY, deltaTime);
    showEntities(cameraX, cameraY);
    showDeathAnimations(cameraX, cameraY, deltaTime);
   
    if (bombExplosionEffect) {
        showBombExplosion(cameraX, cameraY, deltaTime);
    }


    if (fovIsVisible) {
        showFov(playerData.rotation);
    }


    showOnScreenMessage(deltaTime);


    if (!shopIsVisible) {
        showInterface(playerData.inventory, playerData.equippedWeapon, playerData.health);
    }
    if (shopIsVisible) {
        showShop(playerData.inventory, playerData.money);
    }

    renderer.Present();
}

void GameView::showBombExplosion(float cameraX, float cameraY, float deltaTime) {
    Rect src((bomb_explosion_effect.current_frame % 5) * 64, (bomb_explosion_effect.current_frame / 5) * 64, 64, 64);
    Rect dst(cameraX + bomb_explosion_effect.x * BLOCK_SIZE - BLOCK_SIZE * 4, cameraY + bomb_explosion_effect.y * BLOCK_SIZE - BLOCK_SIZE * 4, BLOCK_SIZE * 8, BLOCK_SIZE * 8);
    renderer.Copy(bombAnimationSprite, src, dst);
    bomb_explosion_effect.time_until_next_frame -= deltaTime;
    if (bomb_explosion_effect.time_until_next_frame < 0) {
        bomb_explosion_effect.time_until_next_frame += 0.06;
        bomb_explosion_effect.current_frame++;
    }
    if (bomb_explosion_effect.current_frame >= 25) {
        bombExplosionEffect = false;
    }
}


void GameView::showFov(float angle) {
    int width = renderer.GetOutputWidth() ;
    int height = renderer.GetOutputHeight();
    int tWidth = width * 3;
    int tHeight = height * 3;


    Texture visionTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        tWidth,
        tHeight);
    visionTexture.SetBlendMode(SDL_BLENDMODE_MOD);
    
    renderer.SetTarget(visionTexture);
    
    renderer.SetDrawColor(0, 0, 0, 200);
    renderer.Clear();


    float cx = tWidth / 2.0f;
    float cy = tHeight / 2.0f;

    float half_angle = FOV / 2.0f;
    float radius = 50 * BLOCK_SIZE;

    SDL_Color color = {255, 255, 255, 255};

    std::vector<SDL_Vertex> verts_cone;
    verts_cone.reserve(3);

    verts_cone.push_back({
        .position = {cx, cy},
        .color = color,
        .tex_coord = {0, 0}
    });

    verts_cone.push_back({
        .position = { 
            cx + radius * std::cos(-half_angle), 
            cy + radius * std::sin(-half_angle)
        },
        .color = color,
        .tex_coord = {0, 0}
    });

    verts_cone.push_back({
        .position = {
            cx + radius * std::cos(half_angle),
            cy + radius * std::sin(half_angle)
        },
        .color = color,
        .tex_coord = {0, 0}
    });

    // uso la API raw de SDL porque SDL2pp no tiene RenderGeometry
    SDL_RenderGeometry(renderer.Get(), nullptr, verts_cone.data(), verts_cone.size(), nullptr, 0);


    const int NUM_SEGMENTS = 50;

    float circle_radius = 3 * BLOCK_SIZE;

    std::vector<SDL_Vertex> verts_circle;
    verts_circle.reserve(NUM_SEGMENTS + 2); 


    // vertice central
    verts_circle.push_back({
        .position = {cx, cy},
        .color = color,
        .tex_coord = {0, 0}
    });

    // circulo
    for (int i = 0; i <= NUM_SEGMENTS; ++i) {
        float angle = (2.0f * M_PI * i) / NUM_SEGMENTS;
        float x = cx + circle_radius * std::cos(angle);
        float y = cy + circle_radius * std::sin(angle);
        verts_circle.push_back({
            .position = {x, y},
            .color = color,
            .tex_coord = {0, 0}
        });
    }

    std::vector<int> indices;
    for (int i = 1; i <= NUM_SEGMENTS; ++i) {
        indices.push_back(0);        // centro
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    SDL_RenderGeometry(renderer.Get(), nullptr, verts_circle.data(), verts_circle.size(), indices.data(), indices.size());


    renderer.SetTarget();

    Rect dst = Rect(
        -tWidth / 3,
        -tHeight / 3,
        tWidth,
        tHeight);

    renderer.Copy(visionTexture, NullOpt, dst, angle, SDL_Point{static_cast<int>(tWidth / 2.0f), static_cast<int>(tHeight / 2.0f)}, SDL_FLIP_NONE);
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
    auto game_map = map.get_game_map();
    for (size_t i = 0; i < tiles_map.size(); i++) {
        for (size_t j = 0; j < tiles_map[i].size(); j++) {
            uint16_t tile = tiles_map[i][j];
            MapLegendEntry clip = map.get_tiles_legend(tile);
            Rect src(clip.x, clip.y, CLIP_SIZE, CLIP_SIZE);
            Rect dst(cameraX + j * BLOCK_SIZE, cameraY + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            renderer.Copy(mapTiles, src, dst);
            if (game_map[i][j] == CellType::SpikeSite) {
                renderer.SetDrawColor(255, 255, 0, 40);
                renderer.FillRect(dst);       
            }
        }
    }
}

void GameView::showBullets(float cameraX, float cameraY, float deltaTime) {
    renderer.SetDrawColor(0, 0, 0, 255);
    for (auto it = bullet_effects.begin(); it != bullet_effects.end();) {
        BulletEffect& bullet = *it;

        if (bullet.time_left <= 0) {
            it = bullet_effects.erase(it);
            continue;
        }
        bullet.time_left -= deltaTime; 

        float radians = bullet.angle * M_PI / 180.0 ;
        float dx = std::cos(radians);
        float dy = std::sin(radians);


        float prev_x = bullet.x;
        float prev_y = bullet.y;

        bullet.x += dx * BULLET_SPEED * deltaTime;
        bullet.y += dy * BULLET_SPEED * deltaTime;

        float dot = (bullet.target_x - prev_x) * (bullet.target_x - bullet.x) + (bullet.target_y - prev_y) * (bullet.target_y - bullet.y);

        if (dot < 0.0f) {
            if (bullet.impact == HUMAN) {
                HitEffect blood;
                for (uint32_t i = 0; i < BLOOD_EFFECT_PARTICLES; i++) {
                    Particle blood_drop{

                        bullet.target_x, 
                        bullet.target_y, 
                        randomFloat(BLOOD_PARTICLE_MIN_SPEED, BLOOD_PARTICLE_MAX_SPEED),
                        randomFloat(bullet.angle - BLOOD_PARTICLE_DISPERSION, bullet.angle + BLOOD_PARTICLE_DISPERSION),
                        randomFloat(BLOOD_PARTICLE_MIN_DURATION, BLOOD_PARTICLE_MAX_DURATION)};
                    blood.particles.push_back(blood_drop);
                }
                blood_effects.push_back(blood);
            } else if (bullet.impact == BLOCK) {
                HitEffect sparks;
                for (uint32_t i = 0; i < SPARK_EFFECT_PARTICLES; i++) {
                    Particle spark{
                        bullet.target_x, 
                        bullet.target_y, 
                        randomFloat(SPARK_PARTICLE_MIN_SPEED, SPARK_PARTICLE_MAX_SPEED),
                        randomFloat(0.0f, 360.0f),
                        randomFloat(SPARK_PARTICLE_MIN_DURATION, SPARK_PARTICLE_MAX_DURATION)};
                    sparks.particles.push_back(spark);
                }
                sparks_effects.push_back(sparks);
            }
            it = bullet_effects.erase(it);
            continue;
        }

        int32_t distance_to_target = BLOCK_SIZE * std::sqrt((bullet.x - bullet.target_x) * (bullet.x - bullet.target_x) + (bullet.y - bullet.target_y) * (bullet.y - bullet.target_y));
        int32_t length = std::min(distance_to_target, BULLET_LENGTH) + 1;
        Surface surface(
            0, length, BULLET_THICKNESS, 32,
            0x00FF0000, // Rmask
            0x0000FF00, // Gmask
            0x000000FF, // Bmask
            0xFF000000  // Amask
        );
        surface.FillRect(SDL2pp::NullOpt,
                        SDL_MapRGBA(surface.Get()->format, 255, 240, 0, 255));
        
        
        Texture texture(renderer, surface);
        
        Rect dst(
            cameraX + bullet.x * BLOCK_SIZE, 
            cameraY + bullet.y * BLOCK_SIZE, 
            BULLET_THICKNESS, 
            length);
        
        renderer.Copy(
            texture, 
            NullOpt, 
            dst, 
            bullet.angle - 90.0f, 
            Point(0, BULLET_THICKNESS / 2), 
            SDL_FLIP_NONE);
            
        ++it;
    }
}

void GameView::showBloodEffects(float cameraX, float cameraY, float deltaTime) {
    renderer.SetDrawColor(0, 0, 0, 255);
    for (auto it = blood_effects.begin(); it != blood_effects.end();) {
        HitEffect& hitEffect = *it;

        for (auto pit = hitEffect.particles.begin(); pit != hitEffect.particles.end();) {
            Particle& particle = *pit;
            if (particle.time_left <= 0) {
                pit = hitEffect.particles.erase(pit);
                continue;
            }
            particle.time_left -= deltaTime; 

            float radians = particle.angle * M_PI / 180.0 ;
            float dx = std::cos(radians);
            float dy = std::sin(radians);

            particle.x += dx * std::max(particle.speed * deltaTime + PARTICLE_ACCELERATION/2 * deltaTime * deltaTime, 0.0f);
            particle.y += dy * std::max(particle.speed * deltaTime + PARTICLE_ACCELERATION/2 * deltaTime * deltaTime, 0.0f);
            particle.speed = std::max(particle.speed + PARTICLE_ACCELERATION * deltaTime, 0.0f);

            Rect dst(
                cameraX + particle.x * BLOCK_SIZE, 
                cameraY + particle.y * BLOCK_SIZE, 
                PARTICLE_SIZE, 
                PARTICLE_SIZE);
            
            renderer.Copy(
                bloodTexture, 
                NullOpt, 
                dst, 
                particle.angle - 90.0f, 
                Point(0, PARTICLE_SIZE / 2), 
                SDL_FLIP_NONE);
                
            ++pit;
        }
        if (hitEffect.particles.empty()) {
            it = blood_effects.erase(it);
            continue;
        }
        ++it;
    }
}

void GameView::showSparksEffects(float cameraX, float cameraY, float deltaTime) {
    renderer.SetDrawColor(0, 0, 0, 255);
    for (auto it = sparks_effects.begin(); it != sparks_effects.end();) {
        HitEffect& hitEffect = *it;

        for (auto pit = hitEffect.particles.begin(); pit != hitEffect.particles.end();) {
            Particle& particle = *pit;
            if (particle.time_left <= 0 || particle.speed <= 0) {
                pit = hitEffect.particles.erase(pit);
                continue;
            }
            particle.time_left -= deltaTime; 

            float radians = particle.angle * M_PI / 180.0 ;
            float dx = std::cos(radians);
            float dy = std::sin(radians);

            particle.x += dx * std::max(particle.speed * deltaTime + PARTICLE_ACCELERATION/2 * deltaTime * deltaTime, 0.0f);
            particle.y += dy * std::max(particle.speed * deltaTime + PARTICLE_ACCELERATION/2 * deltaTime * deltaTime, 0.0f);
            particle.speed = std::max(particle.speed + PARTICLE_ACCELERATION * deltaTime, 0.0f);
            


            Rect dst(
                cameraX + particle.x * BLOCK_SIZE, 
                cameraY + particle.y * BLOCK_SIZE, 
                PARTICLE_SIZE, 
                PARTICLE_SIZE);
            
            renderer.Copy(
                sparkTexture, 
                NullOpt, 
                dst, 
                particle.angle - 90.0f, 
                Point(0, PARTICLE_SIZE / 2), 
                SDL_FLIP_NONE);
                
            ++pit;
        }
        if (hitEffect.particles.empty()) {
            it = sparks_effects.erase(it);
            continue;
        }
        ++it;
    }
}
void GameView::showPlayer(float cameraX, float cameraY, Entity player) {
    PlayerData data = std::get<PlayerData>(player.data);
    PlayerInfo info = findPlayerInfo(data.name);
    if (!data.alive) {
        return;
    }
    Rect dst(cameraX + player.x * BLOCK_SIZE - (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2, cameraY + player.y * BLOCK_SIZE - (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE);
    Rect src(0, 0, CLIP_SIZE, CLIP_SIZE);
    if (data.terrorist) { 
        renderer.Copy(getTSkinSprite(info.terroristSkin), src, dst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
    } else {
        renderer.Copy(getCtSkinSprite(info.counterTerroristSkin), src, dst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
    }

    float angleRad = (data.rotation) * M_PI / 180.0f;
    float dx = std::cos(angleRad) * BLOCK_SIZE/2;
    float dy = std::sin(angleRad) * BLOCK_SIZE/2;

    Rect weaponDst(cameraX + player.x * BLOCK_SIZE - (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2 + dx, cameraY + player.y * BLOCK_SIZE - (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2 + dy, BLOCK_SIZE, BLOCK_SIZE);
    switch (data.equippedWeapon) {
        case WeaponType::PRIMARY: {
            renderer.Copy(getWeaponInGameSprite(data.inventory.primary), NullOpt, weaponDst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
            break;
        }
        case WeaponType::SECONDARY: {
            renderer.Copy(glockInGameSprite, NullOpt, weaponDst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
            break;
        }
        case WeaponType::KNIFE: {
            renderer.Copy(knifeInGameSprite, NullOpt, weaponDst, data.rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
            break;
        }
    }
}
void GameView::showWeapon(float cameraX, float cameraY, Entity weapon) {
    WeaponData data = std::get<WeaponData>(weapon.data);
    Rect dst(cameraX + weapon.x * BLOCK_SIZE, cameraY + weapon.y * BLOCK_SIZE, 0, 0);
    if (data.weapon == NONE) {
        return;
    }
    Texture& weaponDroppedSprite = getWeaponDroppedSprite(data.weapon);
    dst.SetH(weaponDroppedSprite.GetHeight());
    dst.SetW(weaponDroppedSprite.GetWidth());
    renderer.Copy(getWeaponDroppedSprite(data.weapon), NullOpt, dst);
}

void GameView::showBomb(float cameraX, float cameraY, Entity bomb) {
    BombData data = std::get<BombData>(bomb.data);
    Rect dst(cameraX + bomb.x * BLOCK_SIZE, cameraY + bomb.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
    if (data.state == BombState::PLANTED) {
        renderer.Copy(bombInGameSprite, NullOpt, dst);
    } else if (data.state == BombState::DROPPED) {
        renderer.Copy(bombInvSprite, NullOpt, dst);
    }
}

void GameView::showEntities(float cameraX, float cameraY) {
    renderer.SetDrawColor(0, 0, 0, 255);
    std::vector<Entity> entities = state.entities;

    for (size_t i = 0; i < entities.size(); i++) {
        switch (entities[i].type) {
            case PLAYER: {
                showPlayer(cameraX, cameraY, entities[i]);
                break;
            }
            case WEAPON: {  
                showWeapon(cameraX, cameraY, entities[i]);
                break;
            }
            case BOMB: {
                showBomb(cameraX, cameraY, entities[i]);
                break;
            }
        }
    }
}

void GameView::showDeathAnimations(float cameraX, float cameraY, float deltaTime) {
    Rect src(0, 0, CLIP_SIZE, CLIP_SIZE);
    for (auto it = death_effects.begin(); it < death_effects.end();) {
        DeathEffect& death = *it;

        if (death.time_left <= 0) {
            it = death_effects.erase(it);
            continue;
        }

        Rect dst(cameraX + death.dead_body_x * BLOCK_SIZE - (1 - PLAYER_WIDTH) * BLOCK_SIZE / 2, cameraY + death.dead_body_y * BLOCK_SIZE - (1 - PLAYER_HEIGHT) * BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE);
        

        death.alpha -= (255 * deltaTime) / DEATH_DURATION;
        if (death.alpha < 0) {
            death.alpha = 0;
        }
        Texture playerTexture = Texture(renderer, death.dead_body_skin);

        playerTexture.SetAlphaMod(death.alpha);

        renderer.Copy(playerTexture, src, dst, death.dead_body_rotation + 90.0f, Point(BLOCK_SIZE / 2, BLOCK_SIZE / 2), SDL_FLIP_NONE);
        death.time_left -= deltaTime;
        ++it;
    }
}

void GameView::showOnScreenMessage(float deltaTime) {
    if (on_screen_message_effect.time_left <= 0) {
        return;
    }
    on_screen_message_effect.time_left -= deltaTime;
    int width = renderer.GetOutputWidth();
    int height = renderer.GetOutputHeight();
    Surface phaseLabel = font.RenderText_Blended(on_screen_message_effect.text, Color(255, 255, 255));
    int labelWidth = phaseLabel.GetWidth() * 2;
    int labelHeight = phaseLabel.GetHeight() * 2;
    const int MARGIN = 20;
    if (labelWidth > width - MARGIN) {
        float ratio = static_cast<float>(phaseLabel.GetHeight()) / static_cast<float>(phaseLabel.GetWidth()); 
        labelWidth = width - MARGIN;
        labelHeight = (width - MARGIN) * ratio;
    };
    Rect phaseLabelRect(
        (width - labelWidth) / 2,
        (height - labelHeight) / 4,
        labelWidth,
        labelHeight
    );

    Texture phaseLabelTexture(renderer, phaseLabel);
    renderer.Copy(
        phaseLabelTexture,  
        NullOpt, 
        phaseLabelRect);
}

void GameView::showInterface(Inventory inventory, WeaponType equippedWeapon, int health) {
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

    Rect container(
        MARGIN,
        MARGIN,
        width - MARGIN * 2,
        height - MARGIN * 2
    );

    renderer.SetDrawColor(255, 255, 255, 0);
    renderer.FillRect(container);




    auto layoutLeftVBox = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren) {
        uint32_t relative_position = 0;
        for (size_t i = 0; i < parentsChildren.size(); i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - label.GetHeight() * text_scale - CONTAINER_MARGIN - relative_position,
            label.GetWidth() * text_scale,
            label.GetHeight() * text_scale 
        );
    };
    Surface healthLabel = font.RenderText_Blended("Health: " + std::to_string(health), Color(255, 255, 255));
    Rect healthLabelRect = layoutLeftVBox(container, healthLabel, {});

    Texture healthLabelTexture(renderer, healthLabel);
    renderer.Copy(
        healthLabelTexture,  
        NullOpt, 
        healthLabelRect);


        
    switch (equippedWeapon) {
        case WeaponType::PRIMARY: {
            Surface ammoLabel = font.RenderText_Blended("Ammo: " + std::to_string(inventory.bulletsPrimary), Color(255, 255, 255));
            Texture ammoLabelTexture(renderer, ammoLabel);
            renderer.Copy(
                ammoLabelTexture,  
                NullOpt, 
                layoutLeftVBox(container, ammoLabel, {healthLabelRect}));
            break;
        }
        case WeaponType::SECONDARY: {
            Surface ammoLabel = font.RenderText_Blended("Ammo: " + std::to_string(inventory.bulletsSecondary), Color(255, 255, 255));
            Texture ammoLabelTexture(renderer, ammoLabel);
            renderer.Copy(
                ammoLabelTexture,  
                NullOpt, 
                layoutLeftVBox(container, ammoLabel, {healthLabelRect}));
            break;
        }
        case WeaponType::KNIFE: {
            break;
        }
    }
    int minutes = static_cast<int>(phaseTimer) / 60;
    int seconds = static_cast<int>(phaseTimer) % 60;
    std::ostringstream oss;
    oss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    Surface timeLabel = font.RenderText_Blended(oss.str(), Color(255, 255, 255));
    Surface roundsLabel = font.RenderText_Blended(
        "Team A " +  std::to_string(state.rounds.roundsWonTeamA) + " - " + std::to_string(state.rounds.roundsWonTeamB) + " Team B", 
        Color(255, 255, 255));

    auto layoutCenterVBox = [=](Rect parent, Surface& label, std::vector<Rect> parentsChildren) {
        uint32_t relative_position = 0;
        for (size_t i = 0; i < parentsChildren.size(); i++) {
            relative_position += parentsChildren[i].GetH() + HEALTH_AMMO_VERTICAL_SPACING;
        }
        return Rect(
            parent.GetX() + parent.GetW()/2 - label.GetWidth() * text_scale / 2,
            parent.GetY() + parent.GetH() - label.GetHeight() * text_scale - CONTAINER_MARGIN - relative_position,
            label.GetWidth() * text_scale,
            label.GetHeight() * text_scale
        );
    };

    Texture timeLabelTexture(renderer, timeLabel);
    Rect timeLabelRect = layoutCenterVBox(container, timeLabel, {});
    renderer.Copy(
        timeLabelTexture,  
        NullOpt, 
        timeLabelRect);
    
    Texture roundsLabelTexture(renderer, roundsLabel);
    renderer.Copy(
        roundsLabelTexture,  
        NullOpt, 
        layoutCenterVBox(container, roundsLabel, {timeLabelRect}));
    
    std::vector<Rect> equipamiento;
    Rect primaryWeaponContainer;
    Rect primaryWeaponSprite;

    auto layoutRightVBox = [=](Rect parent, std::vector<Rect> parentsChildren) {
        uint32_t relative_position = 0;

        for (size_t i = 0; i < parentsChildren.size(); i++) {
            relative_position += parentsChildren[i].GetH() + INVENTORY_VERTICAL_SPACING;
        }


        return Rect(
            parent.GetX() + parent.GetW() - WEAPON_CONTAINER_WIDTH - CONTAINER_MARGIN,
            parent.GetY() + parent.GetH() - WEAPON_CONTAINER_HEIGHT - CONTAINER_MARGIN - relative_position,
            WEAPON_CONTAINER_WIDTH,
            WEAPON_CONTAINER_HEIGHT
        );
    };


    auto createWeaponSprite = [=](Rect parent, Texture& sprite) {

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



    if (inventory.primary != WeaponName::NONE) { // tiene la primaria

        Rect primaryWeaponContainer = layoutRightVBox(container, equipamiento);
        equipamiento.push_back(primaryWeaponContainer);
    
        if (equippedWeapon == WeaponType::PRIMARY) { // tiene la primaria seleccionada
            renderer.SetDrawColor(255, 255, 255, 64);
        } else {
            renderer.SetDrawColor(0, 0, 0, 64);
        }
        renderer.FillRect(primaryWeaponContainer);

        Texture& primaryWeaponSprite = getWeaponInvSprite(inventory.primary);
        Rect primaryWeaponSpriteRect = createWeaponSprite(primaryWeaponContainer, primaryWeaponSprite);
        renderer.Copy(
            primaryWeaponSprite,
            NullOpt,
            primaryWeaponSpriteRect);
    }

    Rect secondaryWeaponContainer = layoutRightVBox(container, equipamiento);
    Rect secondaryWeaponSprite = createWeaponSprite(secondaryWeaponContainer, glockInvSprite);
    equipamiento.push_back(secondaryWeaponContainer);
    if (equippedWeapon == WeaponType::SECONDARY) { // tiene la secundaria seleccionada
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(0, 0, 0, 64);
    }
    renderer.FillRect(secondaryWeaponContainer);
    renderer.Copy(
        glockInvSprite,
        NullOpt,
        secondaryWeaponSprite);
    Rect knifeContainer = layoutRightVBox(container, equipamiento);
    Rect knifeSprite = createWeaponSprite(knifeContainer, knifeInvSprite);

    equipamiento.push_back(knifeContainer);
    if (equippedWeapon == WeaponType::KNIFE) { // tiene el cuchillo seleccionado
        renderer.SetDrawColor(255, 255, 255, 64);
    } else {
        renderer.SetDrawColor(0, 0, 0, 64);
    }
    renderer.FillRect(knifeContainer);
    renderer.Copy(
        knifeInvSprite,
        NullOpt,
        knifeSprite);
        
    if (inventory.has_the_bomb) {  // tiene la bomba
        Rect bombContainer = layoutRightVBox(container, equipamiento);
        Rect bombSprite = createWeaponSprite(bombContainer, bombInvSprite);
        equipamiento.push_back(bombContainer);
        renderer.Copy(
            bombInvSprite,
            NullOpt,
            bombSprite);
    }

}

// TODO: Metodo extremadamente largo, cambiar el planteamiento para que por un lado se creen los Rect
// y se almacenen de algun modo, y cada vez que se actualiza algo (osea, el jugador compra algo o cambia el tamaño de la ventana)
// se vuelvan a crear los Rects. Por otro lado va a ver un metodo que grafique los sprites, los textos,
// y los contenedores de la interfaz dentro de esos rects.

void GameView::showShop(Inventory inv, int money) {
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
        primaryWeaponSectionLabelRect
    );



    Surface BoughtLabel = font.RenderText_Blended("Comprada", Color(255, 255, 255));
    Texture BoughtLabelTexture(renderer, BoughtLabel);

    
    
    int itemContainerX = container.GetX() + CONTAINER_MARGIN;
    for (size_t i = 0; i < shop.weapons.size(); i++) {
        WeaponInfo weaponInfo = findWeaponInfo(shop.weapons[i]);
        int price = weaponInfo.price;
        std::string weaponLabelText = weaponTexts[weaponInfo.name];

        ////////////////////////////////////////////////////
        Rect itemContainer(
            itemContainerX, 
            container.GetY() + CONTAINER_MARGIN + primaryWeaponSectionLabelRect.GetH() + SECTION_VERTICAL_SPACING, 
            (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.weapons.size() - 1))/shop.weapons.size(), 
            container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabelRect.GetH() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
        //////////////////////////////////////////////////
        weaponShopButtons[weaponInfo.name] = {{itemContainer.GetX(), itemContainer.GetY()}, {itemContainer.GetW(), itemContainer.GetH()}};


        if (inv.primary == weaponInfo.name) {
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
        Texture& weaponSprite = getWeaponShopSprite(weaponInfo.name);


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
        renderer.Copy(
            weaponSprite, 
            NullOpt, 
            weaponTextureRect);


        //////////////////////////////////////////////////////////////////////////////////
        Rect boughtLabelRect(
            itemContainer.GetX() + ITEM_CONTAINER_MARGIN + ((itemContainer.GetW() - BoughtLabel.GetWidth() * text_scale - 2 * ITEM_CONTAINER_MARGIN)/2), 
            itemContainer.GetY() + itemContainer.GetH() - BoughtLabel.GetHeight() * text_scale - ITEM_CONTAINER_MARGIN, 
            BoughtLabel.GetWidth() * text_scale, 
            BoughtLabel.GetHeight() * text_scale);
        ///////////////////////////////////////////////////////////////////////////////////
        if (inv.primary == weaponInfo.name) {
            renderer.Copy(
                BoughtLabelTexture, 
                NullOpt, 
                boughtLabelRect);
        }

        itemContainerX = itemContainer.GetX() + itemContainer.GetW() + SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING;
    }
    WeaponInfo primaryWeaponInfo = findWeaponInfo(inv.primary);

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
        (item_container_hbox_width - SECTION_ITEM_CONTAINERS_HORIZONTAL_SPACING * (shop.weapons.size() - 1))/3, 
        container.GetH()/2 - CONTAINER_MARGIN - primaryWeaponSectionLabelRect.GetH() - SECTION_VERTICAL_SPACING - CONTAINER_VERTICAL_SPACING / 2);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    buyPrimaryAmmoButton = {{primaryAmmoContainer.GetX(), primaryAmmoContainer.GetY()}, {primaryAmmoContainer.GetW(), primaryAmmoContainer.GetH()}};



    if (inv.bulletsPrimary >= primaryWeaponInfo.maxAmmo && inv.primary != WeaponName::NONE) {
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

 
    if (inv.bulletsSecondary >= findWeaponInfo(inv.secondary).maxAmmo) {
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

    std::string ammo_price = std::to_string(shop.primaryAmmoPrice);
    Surface primaryAmmoPriceLabel = font.RenderText_Blended("$" + ammo_price, Color(255, 255, 255));
    Rect primaryAmmoPriceLabelRect(
        primaryAmmoContainer.GetX() + primaryAmmoContainer.GetW() - primaryAmmoPriceLabel.GetWidth() * text_scale - ITEM_CONTAINER_MARGIN, 
        primaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        primaryAmmoPriceLabel.GetWidth() * text_scale, 
        primaryAmmoPriceLabel.GetHeight() * text_scale);

    Texture primaryAmmoPriceLabelTexture(renderer, primaryAmmoPriceLabel);
    renderer.Copy(primaryAmmoPriceLabelTexture, NullOpt, primaryAmmoPriceLabelRect);

    ///////////////////////////////////////////////////////////
    std::string ammoBoughtText = "";
    if (inv.primary != WeaponName::NONE) {
        ammoBoughtText = std::to_string(inv.bulletsPrimary) + "/" + std::to_string(primaryWeaponInfo.maxAmmo);
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
    ammo_price = std::to_string(shop.secondaryAmmoPrice);
    Surface secondaryAmmoPriceLabel = font.RenderText_Blended("$" + ammo_price, Color(255, 255, 255));
    Rect secondaryAmmoPriceLabelRect(
        secondaryAmmoContainer.GetX() + secondaryAmmoContainer.GetW() - secondaryAmmoPriceLabel.GetWidth() * text_scale - ITEM_CONTAINER_MARGIN, 
        secondaryAmmoContainer.GetY() + ITEM_CONTAINER_MARGIN, 
        secondaryAmmoPriceLabel.GetWidth() * text_scale, 
        secondaryAmmoPriceLabel.GetHeight() * text_scale);
    //////////////////////////////////////////////////////////////////////////////////////////////

    Texture secondaryAmmoPriceLabelTexture(renderer, secondaryAmmoPriceLabel);
    renderer.Copy(secondaryAmmoPriceLabelTexture, NullOpt, secondaryAmmoPriceLabelRect); 

    ///////////////////////////////////////////////////////////
    
    Surface secondaryAmmoBoughtLabel = font.RenderText_Blended(std::to_string(inv.bulletsSecondary) + "/" + std::to_string(findWeaponInfo(inv.secondary).maxAmmo), Color(255, 255, 255));
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



void GameView::addBulletEffects(Shot shot) {
    for (size_t i = 0; i < shot.bullets.size(); i++) {
        Bullet bullet = shot.bullets[i];
        bullet_effects.push_back(BulletEffect{shot.origin_x, shot.origin_y, bullet.target_x, bullet.target_y, bullet.angle, bullet.impact});
    }
}

void GameView::addDeathEffect(float x, float y, PlayerData& data) {
    PlayerInfo info = findPlayerInfo(data.name);
    if (data.terrorist) {
        Surface& s = getTSkinSpriteSurface(info.terroristSkin);
        death_effects.push_back(DeathEffect{x, y, data.rotation, s});
    } else {
        Surface& s = getCtSkinSpriteSurface(info.counterTerroristSkin);
        death_effects.push_back(DeathEffect{x, y, data.rotation, s});
    }   

}

void GameView::showNewPhaseMessage(Phase phase) {
    std::string text;
    switch (phase) {
        case PURCHASE: {
            text = "Fase de compra";
            break;
        }
        case BOMB_PLANTING: {
            text = "Inicio de ronda";
            break;
        }
        case BOMB_DEFUSING: {
            text = "Bomba plantada";
            break;
        }
        case END_GAME: {
            text = "Final de partida";
        }
        default: {
            break;
        }
    }
    on_screen_message_effect = OnScreenMessageEffect{text};
}
void GameView::showRoundEndMessage(RoundWinner winner) {
    std::string text;
    if (winner.team == 'a') {
        text += "Team A wins! ";
        switch (winner.typeEndRound) {
            case TypeEndRound::BOMB_NOT_PLANTED: {
                text += "Team B ran out of time";
                break;
            }
            case TypeEndRound::DEAD_TEAM: {
                text += "Team B is dead";
                break;
            }
            default: {
                break;
            }
        }
    } else {
        text += "Team B wins! ";
        switch (winner.typeEndRound) {
            case TypeEndRound::BOMB_NOT_PLANTED: {
                text += "Team A ran out of time";
                break;
            }
            case TypeEndRound::DEAD_TEAM: {
                text += "Team A is dead";
                break;
            }
            default: {
                break;
            }
        }
    }
    switch (winner.typeEndRound) {
        case TypeEndRound::BOMB_DEFUSED: {
            text += "Bomb defused";
            break;
        }
        case TypeEndRound::BOMB_EXPLODED: {
            text += "Bomb exploded";
            break;
        }
        default: {
            break;
        }
    }
    on_screen_message_effect = OnScreenMessageEffect{text};
}

void GameView::addBombExplosionEffect(float x, float y) {
    bomb_explosion_effect = BombExplosionEffect{x, y};
    bombExplosionEffect = true;
}


void GameView::switchShopVisibility() {
    shopIsVisible = !shopIsVisible;
}



