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

void Game::updatePlayerPosition(const std::string &name, float x, float y) {
  findPlayerByName(name).setPosition(x, y);
}

std::vector<Entity> Game::getState() { // falta inventario, salud
  std::vector<Entity> state;

  for (const auto &player : players) {
    Entity entity;
    entity.type = PLAYER;
    entity.name = player.getName();
    entity.x = player.getX();
    entity.y = player.getY();
    entity.rotation = player.getRotation();
    state.push_back(entity);
  }

  return state;
}

bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::shoot(const std::string &shooterName) {
  
  Player &shooter = findPlayerByName(shooterName);
  Hitbox hb = shooter.getHitbox();
  /*
  std::cout << "Hitbox de " << shooterName << ": "
          << "desde (" << hb.x << ", " << hb.y << ") "
          << "hasta (" << (hb.x + hb.width) << ", " << (hb.y + hb.height) << ")\n";
  
  */
  float origin_x = hb.x + hb.width / 2.0f;
  float origin_y = hb.y + hb.height / 2.0f;
  float angle = shooter.getRotation();
  float angle_rad = angle * M_PI / 180.0f;

  float max_distance = 30.0f;

  float target_x = origin_x + cos(angle_rad) * max_distance;
  float target_y = origin_y + sin(angle_rad) * max_distance;
  /*
  std::cout << "Disparo desde (" << origin_x << ", " << origin_y << ") hasta ("
          << target_x << ", " << target_y << ")\n";
  */
  Player* closest_player = nullptr;
  float closest_distance = max_distance + 1.0f;
  std::pair<float, float> closest_hit_point;

  for (auto &player : players) {
    if (player.getName() == shooterName)
      continue;

    Hitbox hb = player.getHitbox();

    auto hit_point = hb.intersectsRay(origin_x, origin_y, target_x, target_y);
    if (hit_point) {
      float dx = hit_point->first - origin_x;
      float dy = hit_point->second - origin_y;
      float dist = sqrt(dx*dx + dy*dy);
      if (dist < closest_distance) {
                closest_distance = dist;
                closest_player = &player;
                closest_hit_point = *hit_point;
      }

      shot_event_queue.push(ShotEvent{origin_x, origin_y, hit_point->first, hit_point->second, angle});
    }else{
      shot_event_queue.push(ShotEvent{origin_x, origin_y, target_x, target_y, angle});
    }
  }
  if (closest_player) {
        closest_player->updateHealth(-200.0f);
        /*std::cout << shooterName << " le disparó a " << closest_player->getName()
                  << " en (" << closest_hit_point.first << ", " << closest_hit_point.second << ")\n";
        */
        shot_event_queue.push(ShotEvent{origin_x, origin_y, closest_hit_point.first, closest_hit_point.second, angle});
      
      } else {
      shot_event_queue.push(ShotEvent{origin_x, origin_y, target_x, target_y, angle});
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
  (void)deltaTime;
  // al no estar lo de las balas ya, seria supongo la bomba unicamente
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
  case ActionType::MOVE:{
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->x, data->y, data->deltaTime);
    break;
  }
    
  case ActionType::POINT_TO:{
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);
    break;
  }
    
  case ActionType::SHOOT:{
    shoot(name);
    break;
  }

  default:{
    break;
  }
    
  }
}


ShotEvent Game::shotEventQueuePop() {
  ShotEvent top = shot_event_queue.front();
  shot_event_queue.pop();
  return top;
}

bool Game::shotEventQueueIsEmpty() {
  return shot_event_queue.empty();
}