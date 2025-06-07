#include "game.h"

Game::Game(std::vector<std::vector<CellType>> game_map)
    : map(std::move(game_map)) {
    teamA.setRole(Role::COUNTER_TERRORIST);
    teamB.setRole(Role::TERRORIST);
    spawnTeamTerrorist = map.findSpawnTeam(false); //true para terrorist
    spawnTeamCounter = map.findSpawnTeam(true);
}

bool Game::addPlayer(const std::string &name) {
  players.emplace_back(name);
  Player& player= findPlayerByName(name);
  if (teamA.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    teamA.addPlayer(player);
    player.setRole(teamA.getRole());
    placePlayerInSpawnTeam(player);
    return true;
  }
  if (teamB.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    teamB.addPlayer(player);
    player.setRole(teamB.getRole());
    placePlayerInSpawnTeam(player);
    return true;
  }
  return false;
}

float Game::randomFloatInRange(float min, float max) {
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) + 1) * (max - min);
}

void Game::placePlayerInSpawnTeam(Player& player) {
    std::vector<std::pair<int, int>>& spawn = 
    (player.getRole() == Role::COUNTER_TERRORIST) ? spawnTeamCounter : spawnTeamTerrorist;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    auto [row, col] = spawn[std::rand() % spawn.size()];

    float x = randomFloatInRange(static_cast<float>(col), static_cast<float>(col + 1));
    float y = randomFloatInRange(static_cast<float>(row), static_cast<float>(row + 1));

    player.setPosition(x, y);
}

Player &Game::findPlayerByName(const std::string &name) {
  for (auto &player : players) {
    if (player.getName() == name)
      return player;
  }
  throw std::runtime_error("Player not found");
}

void Game::stopShooting(const std::string &name) {
  Player& player = findPlayerByName(name);
  player.setAlreadyShot(false);
  player.stopShooting();
}

void Game::movePlayer(const std::string &name, float vx, float vy,
                      uint32_t id) {
  Player& player = findPlayerByName(name);
  player.setLastMoveId(id);
  player.updateVelocity(vx, vy);
}

// plant va a ser muy parecido a defuse, solo cambia si el jugador ataca o
// defiende
void Game::plantBomb(
  const std::string &name) { // ahora se planta automaticamente, falta un update y que
                 // despues de x tiempo se cambie a true
  Player& player = findPlayerByName(name);
  if (!player.getHasTheSpike()) { //solo puede tenerla si es atacante
    return;
  }
  auto [x, y, width, height] = getPlayerHitbox(player);
  PlayerCellBounds bounds = getCellBounds(x, y, width, height);

  for (int row = bounds.topCell; row <= bounds.bottomCell; ++row) {
    for (int col = bounds.leftCell; col <= bounds.rightCell; ++col) {
      if (!map.isValidCell(row, col)) {
        continue;
      }
      if (map.isSpikeSite(row, col)) {
        spike.position.x = row;
        spike.position.y = col;
        spike.isPlanted = true; //todo sacar de aca y hacerlo en update
        return;
      }
    }
  }
  return;
}

void Game::stopPlantBomb(const std::string &name) {
  findPlayerByName(name).updateIsPlanting(false);
}

void Game::updatePlanting(const std::string &name) {
  findPlayerByName(name).updateIsPlanting(false);
}

std::tuple<float, float, float, float> Game::getPlayerHitbox(const Player& player) const {
    Hitbox hb = player.getHitbox();
    float x = player.getX();
    float y = player.getY();
    float width = hb.getWidth();
    float height = hb.getHeight();
    return {x, y, width, height};
}


PlayerCellBounds Game::getCellBounds(float x,float y, float width, float height) const {

    float left = x;
    float right = x + width;
    float top = y;
    float bottom = y + height;

    PlayerCellBounds bounds;
    bounds.leftCell = static_cast<int>(std::floor(left));
    bounds.rightCell = static_cast<int>(std::floor(right));
    bounds.topCell = static_cast<int>(std::floor(top));
    bounds.bottomCell = static_cast<int>(std::floor(bottom));

    return bounds;
}

void Game::defuseBomb(const std::string &name) { 
  Player& player = findPlayerByName(name);
  if (player.getRole()==Role::TERRORIST) { //solo puede defusear si es defensor
    return;
  }
  auto [x, y, width, height] = getPlayerHitbox(player);
  PlayerCellBounds bounds = getCellBounds(x, y, width, height);

  for (int row = bounds.topCell; row <= bounds.bottomCell; ++row) {
    for (int col = bounds.leftCell; col <= bounds.rightCell; ++col) {
      if (!map.isValidCell(row, col)) {
        continue;
      }
      if (map.isSpikeSite(row, col)) {
        spike.position.x = row;
        spike.position.y = col;
        spike.isPlanted = false; //todo sacar de aca y hacerlo en update
        return;
      }
    }
  }
  return;
}

void Game::stopDefuse(const std::string &playerName) { (void)playerName; }

void Game::updatePlayerPosition(const std::string &name, float x, float y) {
  findPlayerByName(name).setPosition(x, y);
}

// Manuel: puse esto pq es necesario para sincronizar el cliente
// Esta asi de forma medio hack, despues vos cambialo si queres pero la idea era q la vida del jugador
// se actualice a la pasada por parametro
void Game::updatePlayerHealth(const std::string &name, int health) {
  Player& player = findPlayerByName(name);
  player.updateHealth(health - player.getHealth());
}

void Game::updatePlayerMovement(Player &player, float deltaTime) {
  Hitbox hb = player.getHitbox();

  std::pair<float, float> newPos = player.tryMove(deltaTime);
  float newX = newPos.first;
  float newY = newPos.second;
  float width = hb.getWidth();
  float height = hb.getHeight();
  float originalX = player.getX();
  float originalY = player.getY();
  PlayerCellBounds bounds = getCellBounds(newX, newY, width, height); 

  if (!map.isColliding(bounds)) {
    player.updateMovement(deltaTime, false, false);
    return;
  }
  bounds = getCellBounds(newX, originalY, width, height); 
  if (!map.isColliding(bounds)) {
    player.updateMovement(deltaTime, true, false);
    return;
  } 
  bounds = getCellBounds(originalX, newY, width, height); 
  if (!map.isColliding(bounds)) {
    player.updateMovement(deltaTime, false, true);
    return;
  }
}

void Game::changeWeapon(const std::string &name, WeaponType type) {
  findPlayerByName(name).changeWeapon(type);
}

void Game::buyWeapon(const std::string &name, WeaponName weaponName) {
  Player& player = findPlayerByName(name);
  int price = Store::getWeaponPrice(weaponName);
  if (player.getMoney() >= price) {
    player.updateMoney(-price);
    player.replaceWeapon(weaponName);
    player.changeWeapon(WeaponType::PRIMARY);
    player.resetPrimaryBullets();
  }
}

std::vector<std::pair<WeaponName, int>> Game::getStore() {
  return Store::getStore();
}

void Game::buyBullet(const std::string &name, WeaponType type) {
  Player& player = findPlayerByName(name);
  if (player.getMoney() >= 40) { // constante, todo el cargador
    if (type == WeaponType::PRIMARY) {
      player.resetPrimaryBullets();
    } else {
      player.resetSecondaryBullets();
    }
    player.updateMoney(-40);
  }
}

int Game::checkRoundWinner() {
    if (teamA.getPlayersAlive() > 0 && teamB.getPlayersAlive() == 0) {
        teamA.incrementRoundsWon();
        return 1;
    }
    if (teamB.getPlayersAlive() > 0 && teamA.getPlayersAlive() == 0) {
        teamB.incrementRoundsWon();
        return 2;
    }
    return 0;
}



void Game::updateGamePhase(float deltaTime) { 
  switch (phase) {
    case Phase::PURCHASE:
      purchaseElapsedTime +=deltaTime;
      if(purchaseElapsedTime >= purchaseDuration){
        isBombPlanted=false;
        purchaseElapsedTime = 0.0f;
        phase=Phase::BOMB_PLANTING;
      }
    break;

    case Phase::BOMB_PLANTING:
      plantingElapsedTime +=deltaTime;
      if(checkRoundWinner() !=0 || plantingElapsedTime >= timeToPlantBomb){ //pierden atacantes
        isBombPlanted=false;
        plantingElapsedTime=0.0f;
        phase=Phase::PURCHASE;
        updateRounds();
      }
      if(isBombPlanted){ 
        phase=Phase::BOMB_DEFUSING;
      }
    break;

    case Phase::BOMB_DEFUSING:
      bombElapsedTime +=deltaTime;
      if(checkRoundWinner() !=0 || bombElapsedTime >=timeUntilBombExplode){ //pierden defensores
        bombElapsedTime = 0.0f;
        isBombPlanted = false;
        phase=Phase::PURCHASE;
        updateRounds();
      }

    break;

    default:
    break;
  }
}
void Game::updateRounds(){
  teamA.restartPlayersAlive(); //contador en team y vida de c/u
  teamB.restartPlayersAlive();
  roundNumber+=1;
  roundsUntilRoleChange-=1;
  roundsUntilEndGame-=1;
  if(roundsUntilRoleChange==0){
    teamA.invertRole();
    teamB.invertRole();
    placeTeamsInSpawn();
    
  }
  if(roundsUntilEndGame==0){
    running = false;
  }
}

void Game::placeTeamsInSpawn(){
  for (auto &player : players) {
    placePlayerInSpawnTeam(player);
  }
}

StateGame Game::getState() {
  StateGame state;
  state.phase = phase;

  std::vector<Entity> entities;

  for (const auto &player : players) {
    entities.push_back(getPlayerState(player.getName()));
  }
  Entity entity;
  entity.type = BOMB;
  BombData data;
  data.planted= spike.isPlanted;
  entity.data=data;
  entity.x=spike.position.x;
  entity.y=spike.position.y;
  state.entities = entities;
 
  state.shot= shot_queue;
  return state;
}

Entity Game::getPlayerState(const std::string& name) {
  Player player = findPlayerByName(name);
  Entity entity;
  entity.type = PLAYER;
  entity.x = player.getX();
  entity.y = player.getY();
  Inventory inv;
  inv.primary = player.getPrimaryWeaponName();
  inv.secondary = player.getSecondaryWeaponName();
  inv.bulletsPrimary = player.getBulletsPrimary();
  inv.bulletsSecondary = player.getBulletsSecondary();
  inv.has_the_bomb = player.getHasTheSpike();
  PlayerData data;
  data.equippedWeapon = player.getTypeEquipped();
  data.inventory = inv;
  data.name= player.getName();
  data.rotation = player.getRotation();
  data.lastMoveId = player.getLastMoveId();
  data.health = player.getHealth();
  data.money = player.getMoney();
  data.alive = player.isAlive();
  entity.data = data;
  return entity;
}

bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::makeShot(Player &shooter, const std::string &shooterName) {
  int bullets = shooter.getBulletsPerShoot();
  if (shooter.getTypeEquipped()== WeaponType::PRIMARY){
    shooter.updatePrimaryBullets(-bullets);
  }
  if (shooter.getTypeEquipped()== WeaponType::SECONDARY){
    shooter.updateSecondaryBullets(-bullets);
  }
  Shot shot;
  
  for (int i = 0; i < bullets; i++) {
    auto [maxDistance, originX, originY, targetX, targetY, angle] =
        shooter.shoot();
    shot.origin_x=originX;
    shot.origin_y=originY;
    Bullet bullet;
    
    Player *closestPlayer = nullptr;
    float closestPlayerDist = maxDistance + 1.0f;
    std::pair<float, float> closestHitPoint;

    for (auto &player : players) {
      if (&player == &shooter)
        continue;

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
    bullet.angle=angle;
    if (closestPlayer && closestPlayerDist < wallDist) {
      applyDamageToPlayer(shooter, *closestPlayer, closestPlayerDist);
      bullet.target_x=closestHitPoint.first;
      bullet.target_y=closestHitPoint.second;
      bullet.impact=IMPACT::HUMAN;

    } else if (wallHit) {
      std::cout << shooterName << " disparó y la bala impactó una pared en ("
                << wallPoint.first << ", " << wallPoint.second << ")\n";
      bullet.target_x=wallPoint.first;
      bullet.target_y=wallPoint.second;
      bullet.impact=IMPACT::BLOCK;
    } else {
      bullet.target_x=targetX;
      bullet.target_y=targetY;
      bullet.impact=IMPACT::NOTHING;
    }
    shot.bullets.push_back(bullet);
  }
  shot.weapon=shooter.getEquipped().name;
  shot_queue.push(shot);
  shooter.setAlreadyShot(true);
}

void Game::applyDamageToPlayer(const Player& shooter, Player& target, float distance) {
    std::pair<float, float> damageRange = shooter.getDamageRange();

    float baseDamage = 1000.0f / (distance + 1.0f);
    float clampedDamage = std::clamp(baseDamage, damageRange.first, damageRange.second);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(damageRange.first, damageRange.second);
    float randomDamage = dis(gen);

    float finalDamage = std::min(clampedDamage, randomDamage);
    int finalDamageInt = static_cast<int>(std::ceil(finalDamage));

    target.updateHealth(-finalDamageInt);
}


void Game::shoot(const std::string &shooterName, float deltaTime) {
  Player& shooter = findPlayerByName(shooterName);
  
  if (!shooter.isShooting()) {
    return;
  }
  if (shooter.getShootCooldown() > 0) {
    return;
  }

  if (shooter.getBullets() < shooter.getBulletsPerShoot()) {
      return;
  }


  const Weapon &equipped = shooter.getEquipped();

  if (equipped.burstFire) { // es arma con rafaga
    if (!shooter.getAlreadyShot() ||
        equipped.burstDelay <=
            shooter.getTimeLastBullet()) { // puede disparar otra bala
      // aun quedan balas rafaga
      shooter.updateBurstFireBullets(-1);
      makeShot(shooter, shooterName);
      shooter.resetTimeLastBullet();
      if (shooter.getBurstFireBullets() == 0) { // fin rafaga
        shooter.resetCooldown();
        shooter.updateBurstFireBullets(equipped.bulletsPerBurst);
        shooter.updateTimeLastBullet(deltaTime);
      }
    } else {
      shooter.updateTimeLastBullet(deltaTime);
    }
  } else {
    if (!shooter.getAlreadyShot()) {
      shooter.resetCooldown();
      makeShot(shooter, shooterName);
    }
  }
}

std::optional<std::pair<float, float>>
Game::rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const {
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

    if (!map.isValidCell(row,col)) {
      break;
    }

    if (map.isBlocked(row,col)) { // colisión con la pared
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
  for (auto &player : players) {
    updatePlayerMovement(player, deltaTime);
    player.updateCooldown(deltaTime);
    shoot(player.getName(), deltaTime);
    player.updateAceleration(deltaTime);
  }
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
  case ActionType::MOVE: {
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->vx, data->vy, data->id);
    break;
  }
  case ActionType::POINT_TO: {
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);
    break;
  }

  case ActionType::SHOOT: {
    findPlayerByName(name).startShooting();
    break;
  }
  case ActionType::STOP_SHOOTING: { // en la de rafaga reiniciar rafaga
    stopShooting(name);
    break;
  }
  case ActionType::BUY_BULLET: {
    const BuyBulletAction *data = std::get_if<BuyBulletAction>(&action.data);
    buyBullet(name, data->type);
    break;
  }
  case ActionType::BUY_WEAPON: {
    const BuyWeaponAction *data = std::get_if<BuyWeaponAction>(&action.data);
    buyWeapon(name, data->weapon);
    break;
  }
  case ActionType::CHANGE_WEAPON: {
    const ChangeWeaponAction *data =
        std::get_if<ChangeWeaponAction>(&action.data);
    changeWeapon(name, data->type);
    break;
  }
  case ActionType::PLANT: {
    plantBomb(name);
    break;
  }
  case ActionType::STOP_PLANTING: {
    stopPlantBomb(name);
    break;
  }

  case ActionType::DEFUSE: {
    defuseBomb(name);
    break;
  }
  case ActionType::STOP_DEFUSING: {
    stopDefuse(name);
    break;
  }
  default: {

    break;
  }
  }
}

Shot Game::shotQueuePop() {
  Shot top = shot_queue.front();
  shot_queue.pop();
  return top;
}

bool Game::shotQueueIsEmpty() { return shot_queue.empty(); }

float Game::getX(const std::string &name) {
  return findPlayerByName(name).getX();
}

float Game::getY(const std::string &name) {
  return findPlayerByName(name).getY();
}