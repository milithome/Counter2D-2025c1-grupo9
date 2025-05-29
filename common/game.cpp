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

void Game::stopShooting(const std::string &name){
  Player player= findPlayerByName(name);
  player.stopShooting();
}

void Game::movePlayer(const std::string &name, float vx, float vy){
  Player player= findPlayerByName(name);
  player.updateVelocity(vx, vy);
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
  
  if(shooter.isShooting()){
    if (shooter.getShootCooldown()<=0){
      shooter.resetCooldown();

      int bullets = shooter.getBulletsPerShoot();

      for (int i = 0; i < bullets; i++){ 
        //por cada bala del disparo, para todas menos la m3 es 1
        auto [maxDistance, originX, originY, targetX, targetY, angle] = shooter.shoot();
        Player* closestPlayer = nullptr;
        float closestDistance = maxDistance + 1.0f;
        std::pair<float, float> closestHitPoint;

        for (auto &player : players) {
          if (player.getName() == shooterName){
            continue;
          }

          Hitbox hb = player.getHitbox();

          auto hit_point = hb.intersectsRay(originX, originY, targetX, targetY);
          if (hit_point) {
            float dx = hit_point->first - originX;
            float dy = hit_point->second - originY;
            float dist = sqrt(dx*dx + dy*dy);
            if (dist < closestDistance) {
                      closestDistance = dist;
                      closestPlayer = &player;
                      closestHitPoint = *hit_point;
            }
          }
        }

        if (closestPlayer) {
              std::pair<float, float> damageRange = shooter.getDamageRange();
              //ademas es random en random max y min de cada arma
              float baseDamage = 1000.0f / closestDistance+1.0f;
              float clampedDamage = std::clamp(baseDamage, damageRange.first, damageRange.second);
              std::random_device rd;
              std::mt19937 gen(rd());
              std::uniform_real_distribution<> dis(damageRange.first, damageRange.second);
              float randomDamage = dis(gen);
              float finalDamage = std::min(clampedDamage, randomDamage);
              closestPlayer->updateHealth(-finalDamage);
              std::cout << shooterName << " le disparó a " << closestPlayer->getName()
                        << " en (" << closestHitPoint.first << ", " << closestHitPoint.second << ")\n";
              
              shot_event_queue.push(ShotEvent{originX, originY, closestHitPoint.first, closestHitPoint.second, angle});
            
        } else {
          shot_event_queue.push(ShotEvent{originX, originY, targetX, targetY, angle});
        }
      }
    }
  }else{
    shooter.startShooting();
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
  for (auto player : players){
    player.updateMovement(deltaTime);
    player.updateCooldown(deltaTime);
    shoot(player.getName());
  }
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
  case ActionType::MOVE:{ //cambio a uno solo
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->vx, data->vy);
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
  case ActionType::STOP_SHOOTING:{
    stopShooting(name);
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