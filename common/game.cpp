#include "game.h"
#include "store.h"
#include <cmath>
#include <iostream>

bool Game::addPlayer(const std::string &name) {
  Player newPlayer(name);
  newPlayer.setPosition(10, 10); // ahora mismo hardcodeo una cualquiera
  if (team1.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team1.addPlayer(newPlayer);
    players.push_back(newPlayer);
    //newPlayer.setTeam();
    return true;
  }

  if (team2.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team2.addPlayer(newPlayer);
    players.push_back(newPlayer);
    //newPlayer.setTeam();
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

void Game::movePlayer(const std::string &name, float vx, float vy, uint32_t id){
  Player player= findPlayerByName(name);
  player.setLastMoveId(id);
  player.updateVelocity(vx, vy);
}

void Game::updatePlayerPosition(const std::string &name, float x, float y) {
  findPlayerByName(name).setPosition(x, y);
}
void Game::changeWeapon(const std::string &name, WeaponType type){
  findPlayerByName(name).changeWeapon(type);
}

void Game::buyWeapon(const std::string &name, WeaponName weaponName){
  Player player = findPlayerByName(name);
  int price= Store::getWeaponPrice(weaponName);
  if (player.getMoney()>=price){
    player.updateMoney(-price);
    player.replaceWeapon(weaponName);
    player.changeWeapon(WeaponType::PRIMARY);
  }
}

std::vector<std::pair<WeaponName, int>> Game::getStore(){
  return Store::getStore();
};

void Game::buyBullet(const std::string &name,WeaponType type){
  Player player = findPlayerByName(name);
  if (player.getMoney()>=40){ //constante, todo el cargador
    if(type==WeaponType::PRIMARY){
      player.updatePrimaryBullets();
    }else{
      player.updateSecondaryBullets();
    }
    player.updateMoney(-40);
    
  }
}

std::vector<Entity> Game::getState() {
  std::vector<Entity> state;

  for (const auto &player : players) {
    Entity entity;
    entity.type = PLAYER;
    entity.x = player.getX();
    entity.y = player.getY();
    Inventory inv;
    inv.primary = player.getPrimaryWeaponName();
    inv.secondary = player.getSecondaryWeaponName();
    inv.bulletsPrimary = player.getBulletsPrimary();
    inv.bulletsSecondary = player.getBulletsSecondary();
    PlayerData data;
    data.inventory = inv;
    data.name= player.getName();
    data.rotation = player.getRotation();
    data.lastMoveId = player.getLastMoveId();
    entity.data = data;
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
      // Hitbox hb = shooter.getHitbox();

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
              
              bullet_queue.push(Bullet{originX, originY, closestHitPoint.first, closestHitPoint.second, angle});
            
        } else {
          bullet_queue.push(Bullet{originX, originY, targetX, targetY, angle});
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
    movePlayer(name, data->vx, data->vy, data->id);
    break;
  }
  case ActionType::POINT_TO:{
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);
    break;
  }
    
  case ActionType::SHOOT:{
    findPlayerByName(name).startShooting();
    shoot(name);
    break;
  }
  case ActionType::STOP_SHOOTING:{
    stopShooting(name);
    break;
  }
  case ActionType::BUY_BULLET:{
    const BuyBulletAction*data = std::get_if<BuyBulletAction>(&action.data);
    buyBullet(name, data->type);
    break;
  }
  case ActionType::BUY_WEAPON:{
    const BuyWeaponAction*data = std::get_if<BuyWeaponAction>(&action.data);
    buyWeapon(name, data->weapon);
    break;
  }
  case ActionType::CHANGE_WEAPON:{
    const ChangeWeaponAction*data = std::get_if<ChangeWeaponAction>(&action.data);
    changeWeapon(name, data->type);
    break;
  }
  default:{
    break;
  }
    
  }
}


void Game::bulletQueuePush(Bullet bullet) {
  bullet_queue.push(bullet);
}

Bullet Game::bulletQueuePop() {
  Bullet top = bullet_queue.front();
  bullet_queue.pop();
  return top;
}

bool Game::bulletQueueIsEmpty() {
  return bullet_queue.empty();
}

float Game::getX(const std::string& name) {
  return findPlayerByName(name).getX();
}

float Game::getY(const std::string& name) {
  return findPlayerByName(name).getX();
}