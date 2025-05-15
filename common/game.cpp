#include "game.h"
#include <cmath>
#include <iostream>

bool Game::addPlayer(const std::string &name, const int id) {
  Player new_player(name, id);
  new_player.setPosition(10, 10); // ahora mismo hardcodeo una cualquiera
  if (team1.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team1.addPlayer(new_player);
    players.push_back(new_player);
    return true;
  }

  if (team2.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team2.addPlayer(new_player);
    players.push_back(new_player);
    return true;
  }

  return false;
}

Player &Game::findPlayerById(uint id) {
  for (auto &player : players) {
    if (player.getId() == id)
      return player;
  }
  throw std::runtime_error("Player not found");
}

void Game::movePlayer(uint player_id, int x, int y, float deltaTime) {
  findPlayerById(player_id).move(x, y, deltaTime);
}

std::vector<Entity> Game::getState() { // falta inventario, salud
  std::vector<Entity> state;

  for (const auto &player : players) {
    Entity entity;
    entity.type = PLAYER;
    entity.id = player.getId();
    entity.x = player.getX();
    entity.y = player.getY();
    state.push_back(entity);
  }

  return state;
}

bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::shoot(uint shooter_id) {
  Player &shooter = findPlayerById(shooter_id);
  float origin_x = shooter.getX();
  float origin_y = shooter.getY();

  float angle_rad = shooter.getRotation() * M_PI / 180.0f;

  float max_distance = 100.0f;

  float target_x = origin_x + cos(angle_rad) * max_distance;
  float target_y = origin_y + sin(angle_rad) * max_distance;

  for (auto &player : players) {
    if (player.getId() == shooter_id)
      continue;

    Hitbox hb = player.getHitbox();

    std::cout << "\n";

    if (hb.intersectsRay(origin_x, origin_y, target_x, target_y)) {
      player.updateHealth(-200.0f); // ejemplo
    }
  }
}

void Game::update(float deltaTime) {
  // al no estar lo de las balas ya, seria supongo la bomba unicamente
}
