#include "game_controller.h"
#include <iostream>
#include "common/structures.h"

#define DESYNC_TOLERANCE 0.2

GameController::GameController(GameView& view, Game& game, const std::string& player_name, bool pulse_available)
    : view(view), game(game), soundHandler(pulse_available), player_name(player_name) {
}

void GameController::update(float deltaTime) {

    game.update(deltaTime);


    float player_x = game.getX(player_name);
    float player_y = game.getY(player_name);
    while (!game.shotQueueIsEmpty()) {
        Shot shot = game.shotQueuePop();
        view.addBulletEffects(shot);
        float distance = std::sqrt((player_x - shot.origin_x) * (player_x - shot.origin_x) + (player_y - shot.origin_y) * (player_y - shot.origin_y));
        soundHandler.playShotSound(distance, shot.weapon);
    }


}

void GameController::onKeyPressed(const SDL_Event& event) {
    if (event.key.repeat > 0) {
        return;
    }

    switch (event.key.keysym.sym) {
        case SDLK_m: {
            soundHandler.switchMute();
            break;
        }
        default: {
            break;
        }
    }
    PlayerData data = std::get<PlayerData>(game.getPlayerState(player_name).data);
    if (!data.alive) return;
    switch (event.key.keysym.sym) {
        case SDLK_w: {
            movement_keys_vector[1] -= 1;
            break;
        }
        case SDLK_s: {
            movement_keys_vector[1] += 1;
            break;
        }
        case SDLK_a: {
            movement_keys_vector[0] -= 1;
            break;
        }
        case SDLK_d: {
            movement_keys_vector[0] += 1;
            break;
        }
        case SDLK_b: {
            if (game.getState().phase != PURCHASE) return;
            view.switchShopVisibility();
            shop_open = !shop_open;
            break;
        }
        case SDLK_1: {
            Inventory inv = std::get<PlayerData>(game.getPlayerState(player_name).data).inventory;
            if (inv.primary != WeaponName::NONE) {
                Action action;
                action.type = ActionType::CHANGE_WEAPON;
                action.data = ChangeWeaponAction{WeaponType::PRIMARY};
                game.execute(player_name, action);
                action_queue.push(action);
            }
            break;
        }
        case SDLK_2: {
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::SECONDARY};
            game.execute(player_name, action);
            action_queue.push(action);
            break;
        }
        case SDLK_3: {
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::KNIFE};
            game.execute(player_name, action);
            action_queue.push(action);
            break;
        }
        case SDLK_4: {
            planting = true;
            Inventory inv = std::get<PlayerData>(game.getPlayerState(player_name).data).inventory;
            if (inv.has_the_bomb) {
                Action action{ActionType::PLANT, {}};
                game.execute(player_name, action);
                action_queue.push(action);
                return;
            }
            break;  
        }
        case SDLK_e: {
            defusing = true;
            Action action{ActionType::DEFUSE, {}};
            game.execute(player_name, action);
            action_queue.push(action);
            break;
        }
        default: {
            break;
        }
        
    }
    if (defusing || planting) return;
    if (game.getState().phase == PURCHASE) return;
    if (movement_keys.contains(event.key.keysym.sym)) {
        
        Action action{ActionType::MOVE, MoveAction{++lastMoveId, movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);


        move_actions[lastMoveId] = {game.getX(player_name), game.getY(player_name)};
        game.execute(player_name, action);
    }
}

void GameController::onKeyReleased(const SDL_Event& event) {
    PlayerData data = std::get<PlayerData>(game.getPlayerState(player_name).data);
    if (!data.alive) return;
    switch (event.key.keysym.sym) {
        case SDLK_w: {
            movement_keys_vector[1] += 1;
            break;
        }
        case SDLK_s: {
            movement_keys_vector[1] -= 1;
            break;
        }
        case SDLK_a: {
            movement_keys_vector[0] += 1;
            break;
        }
        case SDLK_d: {
            movement_keys_vector[0] -= 1;
            break;
        }
        case SDLK_4: {
            planting = false;
            Action action{ActionType::STOP_PLANTING, {}};
            game.execute(player_name, action);
            action_queue.push(action);
            break;
        }
        case SDLK_e: {
            defusing = false;
            Action action{ActionType::STOP_DEFUSING, {}};
            game.execute(player_name, action);
            action_queue.push(action);
            break;
        }
    }
    if (defusing || planting) return;
    if (game.getState().phase == PURCHASE) return;
    if (movement_keys.contains(event.key.keysym.sym)) {
        Action action{ActionType::MOVE, MoveAction{++lastMoveId, movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);
 
        move_actions[lastMoveId] = {game.getX(player_name), game.getY(player_name)};
        game.execute(player_name, action);
    }
}

void GameController::onQuitPressed() {
    game.stop();
}

void GameController::onMouseMovement() {
    PlayerData data = std::get<PlayerData>(game.getPlayerState(player_name).data);
    if (!data.alive) return;

    SDL_Point center = view.getCenterPoint();
    SDL_Point mouse_position = SDL_Point();
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    float angle = std::atan2(mouse_position.y - center.y, mouse_position.x - center.x);
    float angleDegrees = angle * 180.0f / 3.14159f;

    Action action{ActionType::POINT_TO, PointToAction{angleDegrees}};
    action_queue.push(action);
    actions.push_back(action);
    game.execute(player_name, action);
}

void GameController::onMouseLeftClick(const SDL_Event& event) {
    PlayerData data = std::get<PlayerData>(game.getPlayerState(player_name).data);
    if (!data.alive) return;
    if (event.button.button == SDL_BUTTON_LEFT) {
        if (shop_open) {
            auto buyPrimaryAmmoButton = view.getBuyPrimaryAmmoButton();
            uint32_t x_range_begining = buyPrimaryAmmoButton.first.first;
            uint32_t x_range_end = x_range_begining + buyPrimaryAmmoButton.second.first;

            uint32_t y_range_begining = buyPrimaryAmmoButton.first.second;
            uint32_t y_range_end = y_range_begining + buyPrimaryAmmoButton.second.second;
            uint32_t x = event.button.x;
            uint32_t y = event.button.y;

            Inventory inv = std::get<PlayerData>(game.getPlayerState(player_name).data).inventory;
            if (inv.primary != WeaponName::NONE  && x > x_range_begining && x < x_range_end && y > y_range_begining && y < y_range_end) {
                Action action;
                action.type = ActionType::BUY_BULLET;
                action.data = BuyBulletAction{WeaponType::PRIMARY};
                game.execute(player_name, action);
                action_queue.push(action);
                return;
            }
            auto buySecondaryAmmoButton = view.getBuySecondaryAmmoButton();
            x_range_begining = buySecondaryAmmoButton.first.first;
            x_range_end = x_range_begining + buySecondaryAmmoButton.second.first;

            y_range_begining = buySecondaryAmmoButton.first.second;
            y_range_end = y_range_begining + buySecondaryAmmoButton.second.second;
            if (x > x_range_begining && x < x_range_end && y > y_range_begining && y < y_range_end) {
                Action action;
                action.type = ActionType::BUY_BULLET;
                action.data = BuyBulletAction{WeaponType::SECONDARY};
                game.execute(player_name, action);
                action_queue.push(action);
                return;
            }
            auto buyWeaponButtons = view.getWeaponShopButtons();
            
            for (const auto& [weapon, buyWeaponButton] : buyWeaponButtons) {
                x_range_begining = buyWeaponButton.first.first;
                x_range_end = x_range_begining + buyWeaponButton.second.first;

                y_range_begining = buyWeaponButton.first.second;
                y_range_end = y_range_begining + buyWeaponButton.second.second;
                if (x > x_range_begining && x < x_range_end && y > y_range_begining && y < y_range_end) {
                    Action action;
                    action.type = ActionType::BUY_WEAPON;
                    action.data = BuyWeaponAction{weapon};
                    game.execute(player_name, action);
                    action_queue.push(action);
                    return;
                }
            }
            return;
        }

        if (game.getState().phase == PURCHASE) return;
        if (defusing || planting) return;

        Action action;
        action.type = ActionType::SHOOT;
        game.execute(player_name, action);
        action_queue.push(action);
    }
}

void GameController::onMouseLeftClickReleased(const SDL_Event& event) {
    PlayerData data = std::get<PlayerData>(game.getPlayerState(player_name).data);
    if (!data.alive) return;
    if (event.button.button == SDL_BUTTON_LEFT) {
        Action action{ActionType::STOP_SHOOTING, {}};
        game.execute(player_name, action);
        // game.stopShooting(player_name);
        action_queue.push(action);
    }
}

Action GameController::actionQueuePop() {
    Action action = action_queue.front();
    action_queue.pop();
    return action;
}

bool GameController::actionQueueIsEmpty() {
    return action_queue.empty();
}

void GameController::updateGameState(StateGame state) {
    float client_player_x_from_server;
    float client_player_y_from_server;
    int bomb_index;
    std::vector<Entity> entities = state.entities;
    for (size_t i = 0; i < entities.size(); i++) {
        Entity entity = entities[i];
        switch (entity.type) {
            case PLAYER: {
                PlayerData serverData = std::get<PlayerData>(entity.data);                               // El estado del jugador que tiene el server
                PlayerData clientData = std::get<PlayerData>(game.getPlayerState(serverData.name).data); // El estado del jugador que tiene el cliente

                if (!clientData.alive) {
                    continue;
                }
                // Si el jugador es el que maneja este cliente manejar la desincronizacion del movimiento
                if (serverData.name == player_name) {
                    client_player_x_from_server = entity.x;
                    client_player_y_from_server = entity.y;
                    if (serverData.lastMoveId != lastMoveIdFromServer) {
                        lastMoveIdFromServer = serverData.lastMoveId;
                        std::pair<float, float> position = move_actions[serverData.lastMoveId];
                        float position_x = position.first;
                        float position_y = position.second;
                        
                        if (std::sqrt((entity.x - position_x) * (entity.x - position_x) + (entity.y - position_y) * (entity.y - position_y)) >= DESYNC_TOLERANCE) {
                            game.updatePlayerPosition(player_name, entity.x, entity.y);
                        }
                    }
                // Si no, actualizar posicion y rotacion normalmente
                } else {
                    game.updatePlayerPosition(serverData.name, entity.x, entity.y);
                    game.updateRotation(serverData.name, serverData.rotation);
                    game.updatePrimaryWeapon(serverData.name, serverData.inventory.primary);
                    ChangeWeaponAction changeWeaponActionData{serverData.equippedWeapon};
                    game.execute(serverData.name, Action{ActionType::CHANGE_WEAPON, changeWeaponActionData});
                }
                // Comparar estados del cliente y server para saber si el jugador acaba de morir
                if (serverData.alive != clientData.alive) {
                    view.addDeathEffect(entity.x, entity.y, serverData.rotation);
                    float player_x = game.getX(player_name);
                    float player_y = game.getY(player_name);
                    float dead_player_x = entity.x;
                    float dead_player_y = entity.y;
                    float distance = std::sqrt((player_x - dead_player_x) * (player_x - dead_player_x) + (player_y - dead_player_y) * (player_y - dead_player_y));
                    soundHandler.playDeathSound(distance);
                }
                // Actualizar vida
                game.updatePlayerHealth(serverData.name, serverData.health);


                break;
            }
            case BOMB: {
                BombData data = std::get<BombData>(entity.data);
                bomb_index = i;
                switch (data.state)
                {
                case BombState::INVENTORY:
                    break;
                case BombState::DROPPED:
                    break;
                case BombState::PLANTED:
                    break;
                case BombState::DEFUSED:
                    break;
                default:
                    break;
                }
                // BombData data = std::get<BombData>(entity.data);
                // game.plantBomb(entity.x, entity.y);
            }
            case WEAPON: {
                // WeaponData data = std::get<WeaponData>(entity.data);
                // game.addDroppedWeapon(entity.x, entity.y, data.weapon);
            }
            //ENTIDAD BOMBA Y ENTIDAD WEAPON EN EL PISO
            default: {
                break;
            }
        }
    }
    std::queue<Shot> shots = state.shots;

    while (!shots.empty()) {
        Shot shot = shots.front();
        shots.pop();
        if (std::abs(shot.origin_y - client_player_y_from_server) <= 0.5 &&
            std::abs(shot.origin_x - client_player_x_from_server) <= 0.5) {
            continue;
        }
        
        view.addBulletEffects(shot);

        float player_x = game.getX(player_name);
        float player_y = game.getY(player_name);
        float distance = std::sqrt((player_x - shot.origin_x) * (player_x - shot.origin_x) + (player_y - shot.origin_y) * (player_y - shot.origin_y));
        soundHandler.playShotSound(distance, shot.weapon);

    }
    if (previous_state.phase != state.phase) {
        if (state.phase == BOMB_PLANTING) view.hideShop();
        if (state.phase == BOMB_DEFUSING) game.plant(entities[bomb_index].x, entities[bomb_index].y);
        if (previous_state.phase == BOMB_DEFUSING && state.phase == END_ROUND) game.defuse();
        shop_open = false;
        view.addNewPhaseEffect(state.phase);
    }
    previous_state = state;
}


bool GameController::processEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_EventType eventType = static_cast<SDL_EventType>(e.type);
        switch (eventType) {
            case SDL_QUIT: {
                onQuitPressed();
                return true;
            }
            case SDL_WINDOWEVENT: {
                onWindowEvent(e);
                break;
            }
            case SDL_KEYDOWN: {
                onKeyPressed(e);
                break;
            }
            case SDL_KEYUP: {
                onKeyReleased(e);
                break;
            }
            case SDL_MOUSEMOTION: {
                onMouseMovement();
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                onMouseLeftClick(e);
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                onMouseLeftClickReleased(e);
                break;
            }
            default: {
                break;
            }
        }
    }
    return false;
}

void GameController::onWindowEvent(const SDL_Event& event) {
    switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            view.resizeHud();
            break;
        }
        default: break;
    }
}