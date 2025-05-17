#include "game.h"
#include <cmath>
#include <iostream>

bool Game::addPlayer(const std::string &name) {
  Player newPlayer(name);
  newPlayer.setPosition(10, 10); // ahora mismo hardcodeo una cualquiera
  if (team1.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team1.addPlayer(newPlayer);
    players.push_back(newPlayer);
    return true;
  }

  if (team2.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team2.addPlayer(newPlayer);
    players.push_back(newPlayer);
    return true;
  }

  return false;
}

Player &Game::findPlayerByName(const std::string &name) {
  for (auto &player : players) {
    if (player.getName() == name)
      return player;
  }
  throw std::runtime_error("Player not found");
}

void Game::movePlayer(const std::string &name, int x, int y, float deltaTime) {
  findPlayerByName(name).move(x, y, deltaTime);
}

std::vector<Entity> Game::getState() { // falta inventario, salud
  std::vector<Entity> state;

  for (const auto &player : players) {
    Entity entity;
    entity.type = PLAYER;
    entity.id = player.getName();
    entity.x = player.getX();
    entity.y = player.getY();
    state.push_back(entity);
  }

  return state;
}

bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::shoot(const std::string &shooterName) {
  Player &shooter = findPlayerByName(shooterName);
  float origin_x = shooter.getX();
  float origin_y = shooter.getY();

  float angle_rad = shooter.getRotation() * M_PI / 180.0f;

  float max_distance = 100.0f;

  float target_x = origin_x + cos(angle_rad) * max_distance;
  float target_y = origin_y + sin(angle_rad) * max_distance;

  for (auto &player : players) {
    if (player.getName() == shooterName)
      continue;

    Hitbox hb = player.getHitbox();

    std::cout << "\n";

    if (hb.intersectsRay(origin_x, origin_y, target_x, target_y)) {
      player.updateHealth(-200.0f); // ejemplo
    }
  }
}

void Game::updateTime(float currentTime) { time = currentTime; }

void Game::updateRotation(const std::string &name, float currentRotation) {
  findPlayerByName(name).setRotation(currentRotation);
}

float Game::getRotation(const std::string &name) {
  return findPlayerByName(name).getRotation();
}

void Game::update(float deltaTime) {
  // al no estar lo de las balas ya, seria supongo la bomba unicamente
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
  case ActionType::MOVE:
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->x, data->y, data->deltaTime);
    break;
  case ActionType::POINT_TO:
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);

  case ActionType::SHOOT:
    shoot(name);
    break;

  default:
    break;
  }
}