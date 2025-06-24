#include "game_controller.h"
#include <iostream>
#include "common/structures.h"


GameController::GameController(GameView& view, const std::string& player_name, Times times, bool pulse_available)
    : view(view), soundHandler(pulse_available), player_name(player_name), times(times) {
}


void GameController::onKeyPressed(const SDL_Event& event) {
    if (event.key.repeat > 0) return;

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
            if (state.phase != PURCHASE) return;
            view.switchShopVisibility();
            shop_open = !shop_open;
            break;
        }
        case SDLK_f: {
            if (!clientPlayerData.alive) return;
            Action action{ActionType::GRAB, {}};
            action_queue.push(action);
            break;
        }
        case SDLK_1: {
            if (!clientPlayerData.alive) return;
            if (defusing || planting) return;
            Inventory inv = clientPlayerData.inventory;
            if (inv.primary != WeaponName::NONE) {
                Action action;
                action.type = ActionType::CHANGE_WEAPON;
                action.data = ChangeWeaponAction{WeaponType::PRIMARY};
                action_queue.push(action);
            }
            break;
        }
        case SDLK_2: {
            if (!clientPlayerData.alive) return;
            if (defusing || planting) return;
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::SECONDARY};
            action_queue.push(action);
            break;
        }
        case SDLK_3: {
            if (!clientPlayerData.alive) return;
            if (defusing || planting) return;
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::KNIFE};
            action_queue.push(action);
            break;
        }
        case SDLK_4: {
            if (!clientPlayerData.alive) return;
            planting = true;
            Inventory inv = clientPlayerData.inventory;
            if (inv.has_the_bomb) {
                Action action{ActionType::PLANT, {}};
                action_queue.push(action);
                return;
            }
            break;  
        }
        case SDLK_e: {
            if (!clientPlayerData.alive) return;
            defusing = true;
            Action action{ActionType::DEFUSE, {}};
            action_queue.push(action);
            break;
        }
        case SDLK_g: {
            view.switchFovVisibility();
            break;
        }
        case SDLK_m: {
            soundHandler.switchMute();
            break;
        }
        default: {
            break;
        }
        
    }
    if (!clientPlayerData.alive) return;
    if (defusing || planting) return;
    if (state.phase == PURCHASE) return;
    if (movement_keys.contains(event.key.keysym.sym)) {
        
        Action action{ActionType::MOVE, MoveAction{movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);
    }
}

void GameController::onKeyReleased(const SDL_Event& event) {
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
            if (!clientPlayerData.alive) return;
            planting = false;
            Action action{ActionType::STOP_PLANTING, {}};
            action_queue.push(action);
            break;
        }
        case SDLK_e: {
            if (!clientPlayerData.alive) return;
            defusing = false;
            Action action{ActionType::STOP_DEFUSING, {}};
            action_queue.push(action);
            break;
        }
    }
    if (!clientPlayerData.alive) return;
    if (defusing || planting) return;
    if (state.phase == PURCHASE) return;
    if (movement_keys.contains(event.key.keysym.sym)) {
        Action action{ActionType::MOVE, MoveAction{movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);
    }
}


void GameController::onMouseMovement() {
    if (!clientPlayerData.alive) return;

    SDL_Point center = view.getCenterPoint();
    SDL_Point mouse_position = SDL_Point();
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    float angle = std::atan2(mouse_position.y - center.y, mouse_position.x - center.x);
    float angleDegrees = angle * 180.0f / 3.14159f;

    Action action{ActionType::POINT_TO, PointToAction{angleDegrees}};
    action_queue.push(action);
    actions.push_back(action);
}

void GameController::onMouseLeftClick(const SDL_Event& event) {
    if (!clientPlayerData.alive) return;

    if (event.button.button == SDL_BUTTON_LEFT) {
        if (shop_open) {
            onMouseLeftClickOnShop(event);
            return;
        }

        if (state.phase == PURCHASE) return;
        if (defusing || planting) return;

        Action action;
        action.type = ActionType::SHOOT;
        action_queue.push(action);
    }
}

void GameController::onMouseLeftClickOnShop(const SDL_Event& event) {
    Inventory inv = clientPlayerData.inventory;
    uint32_t x = event.button.x;
    uint32_t y = event.button.y;

    auto buyPrimaryAmmoButton = view.getBuyPrimaryAmmoButton();

    uint32_t x_range_begining = buyPrimaryAmmoButton.first.first;
    uint32_t x_range_end = x_range_begining + buyPrimaryAmmoButton.second.first;

    uint32_t y_range_begining = buyPrimaryAmmoButton.first.second;
    uint32_t y_range_end = y_range_begining + buyPrimaryAmmoButton.second.second;

    if (inv.primary != WeaponName::NONE && x > x_range_begining && x < x_range_end && y > y_range_begining && y < y_range_end) {
        Action action;
        action.type = ActionType::BUY_BULLET;
        action.data = BuyBulletAction{WeaponType::PRIMARY};
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
            action_queue.push(action);
            return;
        }
    }
}

void GameController::onMouseLeftClickReleased(const SDL_Event& event) {
    if (!clientPlayerData.alive) return;
    if (event.button.button == SDL_BUTTON_LEFT) {
        Action action{ActionType::STOP_SHOOTING, {}};
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

void GameController::updateGameState(StateGame new_state) {
    if (!state_available) {
        state_available = true;
        state = new_state;
        view.setPhaseTimer(times.purchase_duration);
    }
    Entity newClientPlayer;
    PlayerData newClientPlayerData;
    for (size_t i = 0; i < new_state.entities.size(); i++) {
        Entity entity = new_state.entities[i];
        if (entity.type == PLAYER) {
            PlayerData data = std::get<PlayerData>(entity.data);
            if (data.name == player_name) {
                newClientPlayerData = data;
                newClientPlayer = entity;
                break;
            }
        }
    }
    int bomb_index = 0;
    for (size_t i = 0; i < new_state.entities.size(); i++) {
        Entity entity = new_state.entities[i];
        switch (entity.type) {
            case PLAYER: {
                PlayerData currentData = std::get<PlayerData>(entity.data);
                PlayerData previousData = std::get<PlayerData>(state.entities[i].data);

                if (!previousData.alive) {
                    continue;
                }
                // Si el jugador acaba de morir
                if (!currentData.alive) {
                    view.addDeathEffect(entity.x, entity.y, currentData);
                    float player_x = newClientPlayer.x;
                    float player_y = newClientPlayer.y;
                    float dead_player_x = entity.x;
                    float dead_player_y = entity.y;
                    float distance = std::sqrt((player_x - dead_player_x) * (player_x - dead_player_x) + (player_y - dead_player_y) * (player_y - dead_player_y));
                    soundHandler.playDeathSound(distance);
                    Action action{ActionType::MOVE, MoveAction{0, 0}};
                    action_queue.push(action);
                }
                break;
            }
            case BOMB: {
                bomb_index = i;
            }
            default: {
                break;
            }
        }
    }
    std::queue<Shot> shots = new_state.shots;

    while (!shots.empty()) {
        Shot shot = shots.front();
        shots.pop();
        view.addBulletEffects(shot);

        float player_x = newClientPlayer.x;
        float player_y = newClientPlayer.y;
        float distance = std::sqrt((player_x - shot.origin_x) * (player_x - shot.origin_x) + (player_y - shot.origin_y) * (player_y - shot.origin_y));
        soundHandler.playShotSound(distance, shot.weapon);
    }

    if (state.phase != new_state.phase) {
        switch (new_state.phase) {
            case PURCHASE: {
                view.setPhaseTimer(times.purchase_duration);
                break;
            }
            case BOMB_PLANTING: {
                view.setPhaseTimer(times.time_to_plant);
                break;
            }
            case BOMB_DEFUSING: {
                view.setPhaseTimer(times.bomb_duration);
                break;
            }
            case END_ROUND: {
                view.setPhaseTimer(times.time_until_new_round);
                break;
            }
            case END_GAME: {
                view.setPhaseTimer(times.time_until_new_round); // TODO: reemplaza por el tiempo hasta q se cierre el juego
            }
            default: {
                break;
            }
        }



        if (new_state.phase == BOMB_PLANTING) view.hideShop();
        shop_open = false;
        if (new_state.phase == END_ROUND) {
            view.showRoundEndMessage(new_state.rounds.winner);
        } else if (new_state.phase == END_GAME) {
            view.showGameEndMessage(new_state.rounds);
        } else {
            view.showNewPhaseMessage(new_state.phase);
        }

        // BombData data = std::get<BombData>(new_state.entities[bomb_index].data);
        if (new_state.phase == END_ROUND && new_state.rounds.winner.typeEndRound == BOMB_EXPLODED) {
            view.addBombExplosionEffect(new_state.entities[bomb_index].x, new_state.entities[bomb_index].y);

            float player_x = newClientPlayer.x;
            float player_y = newClientPlayer.y;
            float distance = std::sqrt((player_x - new_state.entities[bomb_index].x) * (player_x - new_state.entities[bomb_index].x) + (player_y - new_state.entities[bomb_index].y) * (player_y - new_state.entities[bomb_index].y));
            soundHandler.playBombSound(distance);
        }
        if (new_state.phase == PURCHASE) {
            Action action{ActionType::MOVE, MoveAction{0, 0}};
            action_queue.push(action);
        }

    }


    state = new_state;
    clientPlayerData = newClientPlayerData;
}



bool GameController::processEvents() {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_EventType eventType = static_cast<SDL_EventType>(e.type);
        switch (eventType) {
            case SDL_QUIT: {
                return true;
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

