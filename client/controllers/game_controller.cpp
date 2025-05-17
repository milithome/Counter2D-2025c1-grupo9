#include "game_controller.h"
#include <iostream>
#include "common/structures.h"

GameController::GameController(GameView& view, Game& game, uint player_id)
    : view(view), game(game), player_id(player_id) {
        listen();
}

void GameController::listen() {
    view.bind(SDL_KEYDOWN, [this](const SDL_Event& e) {
        this->onKeyPressed(e);
    });
    view.bind(SDL_KEYUP, [this](const SDL_Event& e) {
        this->onKeyReleased(e);
    });
    view.bind(SDL_QUIT, [this](const SDL_Event&) {
        this->onQuitPressed();
    });
    view.bind(SDL_MOUSEMOTION, [this](const SDL_Event&) {
        this->onMouseMovement();
    });
    view.bind(SDL_MOUSEBUTTONDOWN, [this](const SDL_Event& e) {
        this->onMouseLeftClick(e);
    });
    view.bindLoop([this](float deltaTime) {
        this->update(deltaTime);
    });
}

void GameController::update(float deltaTime) {
    if (movement_keys_vector[0] || movement_keys_vector[1]) {
        game.movePlayer(player_id, movement_keys_vector[0], movement_keys_vector[1], deltaTime);

        // Action action{MOVE, MoveAction{movement_keys_vector[0], movement_keys_vector[1]}};
        // action_queue.push(action);
        // actions.push_back(action);
    }
    //game.updateTime(deltaTime);
}

void GameController::onKeyPressed(const SDL_Event& event) {
    if (event.key.repeat > 0) {
        return;
    }
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
            //game.plantBegin(player_id);
            break;
        }
    }
}


void GameController::onKeyReleased(const SDL_Event& event) {
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
            //game.plantStop();
            break;
        }
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
    game.updateRotation(player_id, angleDegrees);

    // Action action{POINT_TO, PointAction{angleDegrees}};
    // action_queue.push(action);
    // actions.push_back(action);

}

void GameController::onMouseLeftClick(const SDL_Event& event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        SDL_Point center = view.getCenterPoint();
        float angle = std::atan2(event.button.y - center.y, event.button.x - center.x);
        float angleDegrees = angle * 180.0f / 3.14159f;
        (void)angleDegrees; // va a ser usado en otro momento para disparar
        // game.shoot();
    }
}

// void GameController::onMouseLeftClickReleased(const SDL_Event& event) {
//     if (event.button.button == SDL_BUTTON_LEFT) {
//         game.stopShooting();
//     }
// }
Action GameController::actionQueuePop() {
    Action action = action_queue.front();
    action_queue.pop();
    return action;
}

bool GameController::actionQueueIsEmpty() {
    return action_queue.empty();
}