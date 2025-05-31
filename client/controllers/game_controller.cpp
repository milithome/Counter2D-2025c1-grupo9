#include "game_controller.h"
#include <iostream>
#include "common/structures.h"

#define DESYNC_TOLERANCE 0.05

GameController::GameController(GameView& view, Game& game, const std::string& player_name)
    : view(view), game(game), player_name(player_name) {
}


void GameController::update(float deltaTime) {

    game.updateTime(deltaTime);


    while (!game.bulletQueueIsEmpty()) {
        Bullet shot = game.bulletQueuePop();
        view.addShotEffect(shot);
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
        case SDLK_e: {
            //game.plantBegin(player_id);
            break;
        }
        case SDLK_b: {
            view.switchShopVisibility();

        }
    }
    if (movement_keys.contains(event.key.keysym.sym)) {
        Action action{ActionType::MOVE, MoveAction{++lastMoveId, movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);

        move_actions[lastMoveId] = {game.getX(player_name), game.getY(player_name)};
        game.movePlayer(player_name, movement_keys_vector[0], movement_keys_vector[1], lastMoveId);
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
        case SDLK_e: {
            //game.plantStop();
            break;
        }
    }
    if (movement_keys.contains(event.key.keysym.sym)) {
        Action action{ActionType::MOVE, MoveAction{++lastMoveId, movement_keys_vector[0], movement_keys_vector[1]}};
        action_queue.push(action);
        actions.push_back(action);
 
        move_actions[lastMoveId] = {game.getX(player_name), game.getY(player_name)};
        game.movePlayer(player_name, movement_keys_vector[0], movement_keys_vector[1], lastMoveId);
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
    game.updateRotation(player_name, angleDegrees);

    Action action{ActionType::POINT_TO, PointToAction{angleDegrees}};
    action_queue.push(action);
    actions.push_back(action);
}

void GameController::onMouseLeftClick(const SDL_Event& event, float deltaTime) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        game.shoot(player_name, deltaTime);
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


void GameController::processEvents(float deltaTime) {
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
                onMouseLeftClick(e, deltaTime);
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