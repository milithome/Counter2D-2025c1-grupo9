#include "game.h"

bool Game::addPlayer(const std::string &name) {
  Player newPlayer(name);
  newPlayer.setPosition(2, 2); // ahora mismo hardcodeo una cualquiera
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
  Player &player= findPlayerByName(name);
  player.setAlreadyShot(false);
  player.stopShooting();
}

void Game::movePlayer(const std::string &name, float vx, float vy, uint32_t id){
  Player &player= findPlayerByName(name);
  player.setLastMoveId(id);
  player.updateVelocity(vx, vy);
}

bool Game::isColliding(float x, float y, float width, float height) const {
    const float epsilon = 0.1f;
    float left = x + epsilon;
    float right = x + width - epsilon;
    float top = y + epsilon;
    float bottom = y + height - epsilon;

    int leftCell = static_cast<int>(std::floor(left));
    int rightCell = static_cast<int>(std::floor(right));
    int topCell = static_cast<int>(std::floor(top));
    int bottomCell = static_cast<int>(std::floor(bottom));

    for (int row = topCell; row <= bottomCell; ++row) {
      for (int col = leftCell; col <= rightCell; ++col) {
        if (row < 0 || col < 0 || row >= static_cast<int>(map.size()) || col >= static_cast<int>(map[row].size())){
          continue;
        } 
        if (map[row][col] == CellType::Blocked) {
          return true;
        }
      }
    }
    return false;
}
//plant va a ser muy parecido a defuse, solo cambia si el jugador ataca o defiende
void Game::plantBomb(const std::string &name) { //ahora se planta automaticamente, falta un update y que despues de x tiempo se cambie a true
    Player player = findPlayerByName(name);
    if(!player.getHasTheSpike()){ //falta agregar si puede defusear o sea si esta defendiendo
      return;
    }
    Hitbox hb = player.getHitbox();
    float x = player.getX();
    float y = player.getY();
    float width = hb.getWidth();
    float height = hb.getHeight();

    float left = x;
    float right = x + width;
    float top = y;
    float bottom = y + height;

    int leftCell = static_cast<int>(std::floor(left));
    int rightCell = static_cast<int>(std::floor(right));
    int topCell = static_cast<int>(std::floor(top));
    int bottomCell = static_cast<int>(std::floor(bottom));

    for (int row = topCell; row <= bottomCell; ++row) {
      for (int col = leftCell; col <= rightCell; ++col) {
        if (row < 0 || col < 0 || row >= static_cast<int>(map.size()) || col >= static_cast<int>(map[row].size())){
          continue;
        } 
        if (map[row][col] == CellType::SpikeSite) {
            spike.position.x=row;
            spike.position.y=col;
            spike.isPlanted=true;
          return;
        }
      }
    }
    return;
}

void Game::stopPlantBomb(const std::string &name){
  findPlayerByName(name).updateIsPlanting(false);
}

void Game::updatePlanting(const std::string &name){
  findPlayerByName(name).updateIsPlanting(false);
}

void Game::defuseBomb(const std::string& playerName) {
    (void)playerName;
}

void Game::stopDefuse(const std::string& playerName) {
    (void)playerName;
}


void Game::updatePlayerPosition(const std::string &name, float x, float y) {
  findPlayerByName(name).setPosition(x, y);
}

void Game::updatePlayerMovement(Player& player, float deltaTime) {
    Hitbox hb = player.getHitbox();

    std::pair<float, float> newPos = player.tryMove(deltaTime);
    float newX = newPos.first;
    float newY = newPos.second;
    float width = hb.getWidth();
    float height = hb.getHeight();

    float originalX = player.getX();
    float originalY = player.getY();

    if (!isColliding(newX, newY, width, height)) {
        player.updateMovement(deltaTime, false, false);
        return;
    }

    if (!isColliding(newX, originalY, width, height)) {
        player.updateMovement(deltaTime, true, false);
    }
    else if (!isColliding(originalX, newY, width, height)) {
        player.updateMovement(deltaTime, false, true);
    }
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
}

void Game::buyBullet(const std::string &name,WeaponType type){
  Player &player = findPlayerByName(name);
  if (player.getMoney()>=40){ //constante, todo el cargador
    if(type==WeaponType::PRIMARY){
      player.updatePrimaryBullets();
    }else{
      player.updateSecondaryBullets();
    }
    player.updateMoney(-40);
    
  }
}

StateGame Game::getState() {
  StateGame state;
  state.phase = phase;
  
  std::vector<Entity> entities;

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
    data.health = player.getHealth();
    data.money = player.getMoney();
    entity.data = data;
    entities.push_back(entity);
  }
  state.entities = entities;
  state.bullets = bullet_queue;
  return state;
}

bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::makeShot(Player& shooter, const std::string& shooterName) {
    //devolver a que le pegó, si a una pared o a un personaje
    int bullets = shooter.getBulletsPerShoot();
    
    for (int i = 0; i < bullets; i++) {
    auto [maxDistance, originX, originY, targetX, targetY, angle] = shooter.shoot();
    
    Player* closestPlayer = nullptr;
    float closestPlayerDist = maxDistance + 1.0f;
    std::pair<float, float> closestHitPoint;

    for (auto& player : players) {
        if (&player == &shooter) continue;

        Hitbox hb = player.getHitbox();
        auto hit_point = hb.intersectsRay(originX, originY, targetX, targetY);
        if (hit_point) {
            float dx = hit_point->first - originX;
            float dy = hit_point->second - originY;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < closestPlayerDist) {
                closestPlayerDist = dist;
                closestPlayer = &player;
                closestHitPoint = *hit_point;
            }
        }
    }

    auto wallHit = rayHitsWall(originX, originY, targetX, targetY, maxDistance);

    float wallDist = maxDistance + 1.0f;
    std::pair<float, float> wallPoint;

    if (wallHit) {
        wallPoint = *wallHit;
        float dx = wallPoint.first - originX;
        float dy = wallPoint.second - originY;
        wallDist = std::sqrt(dx * dx + dy * dy);
    }

    if (closestPlayer && closestPlayerDist < wallDist) {
        std::pair<float, float> damageRange = shooter.getDamageRange();

        float baseDamage = 1000.0f / (closestPlayerDist + 1.0f);
        float clampedDamage = std::clamp(baseDamage, damageRange.first, damageRange.second);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(damageRange.first, damageRange.second);
        float randomDamage = dis(gen);

        float finalDamage = std::min(clampedDamage, randomDamage);

        closestPlayer->updateHealth(-finalDamage);
        std::cout << shooterName << " le disparó a " << closestPlayer->getName()
                  << " en (" << closestHitPoint.first << ", " << closestHitPoint.second << ")\n";

        bullet_queue.push(Bullet{originX, originY, closestHitPoint.first, closestHitPoint.second, angle, IMPACT::HUMAN});
    } else if (wallHit) {
        std::cout << shooterName << " disparó y la bala impactó una pared en ("
                  << wallPoint.first << ", " << wallPoint.second << ")\n";
        bullet_queue.push(Bullet{originX, originY, wallPoint.first, wallPoint.second, angle, IMPACT::BLOCK});
    } else {
        bullet_queue.push(Bullet{originX, originY, targetX, targetY, angle, IMPACT::NOTHING});
    }
  }
  shooter.setAlreadyShot(true);
}

void Game::shoot(const std::string &shooterName, float deltaTime) {
  Player &shooter = findPlayerByName(shooterName);
  if (!shooter.isShooting()) {
      return;
  }
  if (shooter.getShootCooldown() > 0) {
      return;
  }

  const Weapon& equipped = shooter.getEquipped();
    
  if (equipped.burstFire) { //es arma con rafaga
    if (!shooter.getAlreadyShot() || equipped.burstDelay <= shooter.getTimeLastBullet()) { //puede disparar otra bala
      //aun quedan balas rafaga
      shooter.updateBurstFireBullets(-1);
      makeShot(shooter, shooterName);
      shooter.resetTimeLastBullet();
      if (shooter.getBurstFireBullets() == 0) { //fin rafaga
        shooter.resetCooldown();
        shooter.updateBurstFireBullets(equipped.bulletsPerBurst);
        shooter.updateTimeLastBullet(deltaTime);
      }
    }else {
      shooter.updateTimeLastBullet(deltaTime);
    }
  }else{
    if (!shooter.getAlreadyShot()){
      shooter.resetCooldown();
      makeShot(shooter, shooterName);
    } 
  }
}

std::optional<std::pair<float, float>> Game::rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = std::sqrt(dx * dx + dy * dy);

    int steps = static_cast<int>(length * 50);
    float stepX = dx / steps;
    float stepY = dy / steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps && i * std::hypot(stepX, stepY) <= maxDist; ++i) {
        int row = static_cast<int>(std::floor(y));
        int col = static_cast<int>(std::floor(x));

        if (row < 0 || col < 0 || row >= static_cast<int>(map.size()) || col >= static_cast<int>(map[row].size())) {
            break;
        }

        if (map[row][col] == CellType::Blocked) { // colisión con la pared
          return std::make_pair(x, y);  
        }

        x += stepX;
        y += stepY;
    }

    return std::nullopt;
}


void Game::updateTime(float currentTime) { time = currentTime; }

void Game::updateRotation(const std::string &name, float currentRotation) {
  findPlayerByName(name).setRotation(currentRotation);
}

float Game::getRotation(const std::string &name) {
  return findPlayerByName(name).getRotation();
}

void Game::update(float deltaTime) {
  for (auto &player : players){
    updatePlayerMovement(player, deltaTime);
    player.updateCooldown(deltaTime);
    shoot(player.getName(), deltaTime);
    player.updateAceleration(deltaTime);
  }
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
    case ActionType::MOVE:{ 
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
      break;
    }
    case ActionType::STOP_SHOOTING:{ //en la de rafaga reiniciar rafaga
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
    case ActionType::PLANT:{
      plantBomb(name);
      break;
    }
    case ActionType::STOP_PLANTING:{
      stopPlantBomb(name);
      break;
    }

    case ActionType::DEFUSE:{
      defuseBomb(name);
      break;
    }
    case ActionType::STOP_DEFUSING:{
      stopDefuse(name);
      break;
    }
    default:{

      break;
    }
    
  }
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
  return findPlayerByName(name).getY();
}