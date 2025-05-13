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

std::vector<Entity> Game::getState() {
  std::vector<Entity> state;

  for (const auto &player : players) {
    Entity entity;
    entity.type = PLAYER;
    entity.id = player.getId();
    entity.x = player.getX();
    entity.y = player.getY();
    entity.rotation = player.getRotation();
    state.push_back(entity);
  }

  return state;
}

bool Game::isRunning(){
  return running;
}

void Game::stop(){
  running=false;
}

void Game::updateTime(float currentTime){
  time=currentTime;
}

void Game::updateRotation(uint player_id, float currentRotation){
  findPlayerById(player_id).setRotation(currentRotation);
}

float Game::getRotation(uint player_id){
  return findPlayerById(player_id).getRotation();
}

void Game::update(float deltaTime){ 
  for (auto& bullet : activeBullets) {
    bullet.update(deltaTime);

    for (auto& player : players) {
      Hitbox bulletHitbox{bullet.getX(), bullet.getY(), BULLET_WIDTH, BULLET_HEIGHT};
      if (bulletHitbox.intersects(player.getHitbox())) { 
        //tendria que saber el daño, propongo que bala tenga posicion
        //inicial y final para calcular distancia
        bullet.destroy();
        //player.updateHealth();
      }
    }
  }

  activeBullets.erase(std::remove_if(activeBullets.begin(), activeBullets.end(),
                                     [](const Bullet& b) { return b.isDestroyed(); }),
                      activeBullets.end());
}


void Game::shoot(uint player_id) {
  Player& player = findPlayerById(player_id);
  float angle = player.getRotation();

  float radians = angle * M_PI / 180.0f;
  float offset = PLAYER_WIDTH / 2.0f;

  float bulletX = player.getX() + std::cos(radians) * offset;
  float bulletY = player.getY() + std::sin(radians) * offset;

  Bullet bullet(bulletX, bulletY, angle);
  activeBullets.push_back(bullet);
}

