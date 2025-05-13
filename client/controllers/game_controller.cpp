#include "game_controller.h"
#include <iostream>

GameController::GameController(GameView& view, Game& game, uint player_id)
    : view(view), game(game), player_id(player_id) {
        listen();
}

void GameController::listen() {
    view.bind(SDL_KEYDOWN, [this](const SDL_Event& e, float deltaTime, bool is_last_event_this_frame) {
        this->onKeyPressed(e, deltaTime, is_last_event_this_frame);
    });
<<<<<<< Updated upstream
    view.bind(SDL_QUIT, [this](const SDL_Event& e, Uint32 deltaTime, bool) {
        this->onQuitPressed(e, deltaTime);
=======
    view.bind(SDL_KEYUP, [this](const SDL_Event& e, float deltaTime, bool is_last_event_this_frame) {
        this->onKeyReleased(e, deltaTime, is_last_event_this_frame);
    });
    view.bind(SDL_QUIT, [this](const SDL_Event& e, float, bool) {
        this->onQuitPressed(e);
>>>>>>> Stashed changes
    });
    view.bind(SDL_MOUSEMOTION, [this](const SDL_Event& e, float deltaTime, bool) {
        this->onMouseMovement(e, deltaTime);
    });
    view.bind(SDL_MOUSEBUTTONDOWN, [this](const SDL_Event& e, float deltaTime, bool is_last_event_this_frame) {
        this->onMouseLeftClick(e, deltaTime, is_last_event_this_frame);
    });
    view.bindLoop([this](float deltaTime) {
        this->update(deltaTime);
    });
}

void GameController::update(float deltaTime) {
    game.movePlayer(player_id, movement_keys_vector[0], movement_keys_vector[1], deltaTime);
}

void GameController::onKeyPressed(const SDL_Event& event, float deltaTime, bool is_last_event_this_frame) {
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
<<<<<<< Updated upstream
    if (is_last_event_this_frame) {
        if (!pressed_plant_button_this_frame) {
            float length = std::sqrt(movement_keys_vector[0] * movement_keys_vector[0] + movement_keys_vector[1] * movement_keys_vector[1]);
            if (length != 0.0f) {
                movement_keys_vector[0] /= length;
                movement_keys_vector[1] /= length;
            }
            player.move(movement_keys_vector[0] * deltaTime, movement_keys_vector[1] * deltaTime);
            movement_keys_vector.clear();
            frames_elapsed_since_planting_started = 0;
        } else {
            frames_elapsed_since_planting_started += 1;
            if (frames_elapsed_since_planting_started >= FRAMES_UNTIL_SUCCESSFUL_PLANT) {
                //game.plantBomb(player.getX(), player.getY());
            }
=======
    (void)is_last_event_this_frame;
    (void)deltaTime;
}


void GameController::onKeyReleased(const SDL_Event& event, float deltaTime, bool is_last_event_this_frame) {
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
>>>>>>> Stashed changes
        }
    }
    (void)deltaTime;
    (void)is_last_event_this_frame;
}

void GameController::onQuitPressed(const SDL_Event& event, Uint32 deltaTime) {
    //game.stop();
}

<<<<<<< Updated upstream
void GameController::onMouseMovement(const SDL_Event& event, Uint32 deltaTime) {
=======
void GameController::onMouseMovement(const SDL_Event& event, float deltaTime) {
    (void)event;
    (void)deltaTime;
>>>>>>> Stashed changes
    SDL_Point center = view.getCenterPoint();
    SDL_Point mouse_position = SDL_Point();
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
    float angle = std::atan2(mouse_position.y - center.y, mouse_position.x - center.x);
    float angleDegrees = angle * 180.0f / 3.14159f;
    // deberia rotar al jugador
}

<<<<<<< Updated upstream
void GameController::onMouseLeftClick(const SDL_Event& event, Uint32 deltaTime, bool is_last_event_this_frame) {
=======
void GameController::onMouseLeftClick(const SDL_Event& event, float deltaTime, bool is_last_event_this_frame) {
    (void)deltaTime;
>>>>>>> Stashed changes
    if (event.button.button == SDL_BUTTON_LEFT) {
        SDL_Point center = view.getCenterPoint();
        float angle = std::atan2(event.button.y - center.y, event.button.x - center.x);
        float angleDegrees = angle * 180.0f / 3.14159f;
        // logica complicada q deberia hacer cosas distintas dependiendo si el arma es automatica
        // semiautomatica o manual
        pressed_shoot_button_this_frame = true;
    }
    if (is_last_event_this_frame) {
        pressed_plant_button_this_frame = false;
    }
}
