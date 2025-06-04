#include "game_controller.h"
#include <iostream>
#include "common/structures.h"

#define DESYNC_TOLERANCE 0.05

GameController::GameController(GameView& view, Game& game, const std::string& player_name)
    : view(view), game(game), player_name(player_name) {
}


void GameController::update(float deltaTime) {

    game.update(deltaTime);

    while (!game.bulletQueueIsEmpty()) {
        view.addShotEffect(game.bulletQueuePop());
    }
}

void GameController::onKeyPressed(const SDL_Event& event) {
    if (event.key.repeat > 0) {
        return;
    }
    switch (event.key.keysym.sym) {
        case SDLK_UP: {
            movement_keys_vector[1] -= 1;
            break;
        }
        case SDLK_DOWN: {
            movement_keys_vector[1] += 1;
            break;
        }
        case SDLK_LEFT: {
            movement_keys_vector[0] -= 1;
            break;
        }
        case SDLK_RIGHT: {
            movement_keys_vector[0] += 1;
            break;
        }
        case SDLK_b: {
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
            }
            break;
        }
        case SDLK_2: {
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::SECONDARY};
            game.execute(player_name, action);
            break;
        }
        case SDLK_3: {
            Action action;
            action.type = ActionType::CHANGE_WEAPON;
            action.data = ChangeWeaponAction{WeaponType::KNIFE};
            game.execute(player_name, action);
            break;
        }
        case SDLK_4: {
            Inventory inv = std::get<PlayerData>(game.getPlayerState(player_name).data).inventory;
            if (inv.has_the_bomb) {
                // Action action{ActionType::PLANT};
                // game.execute(player_name, action);
                return;
            }
            break;  
        }
        case SDLK_e: {
            // Action action{ActionType::DEFUSE};
            // game.execute(player_name, action);
            break;
        }
        default: {
            break;
        }
    }
    if (movement_keys.contains(event.key.keysym.sym)) {
        Action action{ActionType::MOVE, MoveAction{++lastMoveId, movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);


        move_actions[lastMoveId] = {game.getX(player_name), game.getY(player_name)};
        game.execute(player_name, action);
    }
}


void GameController::onKeyReleased(const SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_UP: {
            movement_keys_vector[1] += 1;
            break;
        }
        case SDLK_DOWN: {
            movement_keys_vector[1] -= 1;
            break;
        }
        case SDLK_LEFT: {
            movement_keys_vector[0] += 1;
            break;
        }
        case SDLK_RIGHT: {
            movement_keys_vector[0] -= 1;
            break;
        }
        case SDLK_4: {
            // Action action{ActionType::STOP_PLANTING};
            // game.execute(player_name, action);
            break;
        }
        case SDLK_e: {
            // Action action{ActionType::STOP_DEFUSING};
            // game.execute(player_name, action);
            break;
        }
    }
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
                }
            }
            return;
        }


        Action action;
        action.type = ActionType::SHOOT;
        game.execute(player_name, action);
    }
}

void GameController::onMouseLeftClickReleased(const SDL_Event& event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        game.stopShooting(player_name);
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
    std::vector<Entity> entities = state.entities;
    for (size_t i = 0; i < entities.size(); i++) {
        Entity entity = entities[i];
        switch (entity.type) {
            case PLAYER: {
                PlayerData data = std::get<PlayerData>(entity.data);
                if (data.name == player_name) {
                    if (data.lastMoveId == lastMoveIdFromServer) {
                        continue;
                    } else {
                        lastMoveIdFromServer = data.lastMoveId;
                    }
                    std::pair<float, float> position = move_actions[data.lastMoveId];
                    float position_x = position.first;
                    float position_y = position.second;
                    
                    if (std::sqrt((entity.x - position_x) * (entity.x - position_x) + (entity.y - position_y) * (entity.y - position_y)) >= DESYNC_TOLERANCE) {
                        game.updatePlayerPosition(player_name, entity.x, entity.y);
                    }
                    continue;
                }
                game.updatePlayerPosition(data.name, entity.x, entity.y);
                game.updateRotation(data.name, data.rotation);
                break;
            }
            default: {
                break;
            }
        }
    }

    std::queue<Bullet> bullets = state.bullets;
    while (!bullets.empty()) {
        Bullet bullet = bullets.front();
        bullets.pop();
        view.addShotEffect(bullet);
    }
    Phase phase = state.phase;
    (void)phase;
}


void GameController::processEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_EventType eventType = static_cast<SDL_EventType>(e.type);
        switch (eventType) {
            case SDL_KEYDOWN: {
                onKeyPressed(e);
                break;
            }
            case SDL_KEYUP: {
                onKeyReleased(e);
                break;
            }
            case SDL_QUIT: {
                onQuitPressed();
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
}