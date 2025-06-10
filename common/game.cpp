#include "game.h"

Game::Game(std::vector<std::vector<CellType>> game_map)
    : map(std::move(game_map))
{
  teamA.setRole(Role::COUNTER_TERRORIST);
  teamB.setRole(Role::TERRORIST);
  spawnTeamTerrorist = map.findSpawnTeam(false); // true para terrorist
  spawnTeamCounter = map.findSpawnTeam(true);
  spike.state = BombState::INVENTORY;
}

bool Game::addPlayer(const std::string &name)
{
  players.emplace_back(name);
  Player &player = findPlayerByName(name);
  if (teamA.getTeamSize() < teamB.getTeamSize() && teamA.getTeamSize() < MAX_PLAYERS_PER_TEAM)
  {
    teamA.addPlayer(player);
    player.setRole(teamA.getRole());
    placePlayerInSpawnTeam(player);
    return true;
  }
  if (teamB.getTeamSize() < MAX_PLAYERS_PER_TEAM)
  {
    teamB.addPlayer(player);
    player.setRole(teamB.getRole());
    placePlayerInSpawnTeam(player);
    return true;
  }
  return false;
}

float Game::randomFloatInRange(float min, float max)
{
  return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) + 1) * (max - min);
}

void Game::placePlayerInSpawnTeam(Player &player)
{
  std::vector<std::tuple<int, int, bool>> &spawn =
      (player.getRole() == Role::COUNTER_TERRORIST) ? spawnTeamCounter : spawnTeamTerrorist;

  for (auto &pos : spawn)
  {
    int row = std::get<0>(pos);
    int col = std::get<1>(pos);
    bool used = std::get<2>(pos);

    if (!used)
    {
      float x = static_cast<float>(col) + 0.1f;
      float y = static_cast<float>(row) + 0.1f;

      std::get<2>(pos) = true;
      player.setPosition(x, y);

      return;
    }
  }
}
void Game::resetSpawn()
{

  for (auto &pos : spawnTeamCounter)
  {
    std::get<2>(pos) = false;
  }
  for (auto &pos : spawnTeamTerrorist)
  {
    std::get<2>(pos) = false;
  }
}

Player &Game::findPlayerByName(const std::string &name)
{
  for (auto &player : players)
  {
    if (player.getName() == name)
      return player;
  }
  throw std::runtime_error("Player not found");
}

void Game::stopShooting(const std::string &name)
{
  Player &player = findPlayerByName(name);
  player.setAlreadyShot(false);
  player.stopShooting();
}

void Game::movePlayer(const std::string &name, float vx, float vy, uint32_t id)
{
  Player &player = findPlayerByName(name);
  player.setLastMoveId(id);
  player.updateVelocity(vx, vy);
}

void Game::plantBomb(
    const std::string &name)
{
  timePlanting = 0.0f;
  Player &player = findPlayerByName(name);
  if (!player.getHasTheSpike())
  {
    return;
  }

  auto [x, y, width, height] = getPlayerHitbox(player);
  PlayerCellBounds bounds = getCellBounds(x, y, width, height);

  for (int row = bounds.topCell; row <= bounds.bottomCell; ++row)
  {
    for (int col = bounds.leftCell; col <= bounds.rightCell; ++col)
    {
      if (!map.isValidCell(row, col))
      {
        continue;
      }
      if (map.isSpikeSite(row, col))
      {
        findPlayerByName(name).updateIsPlanting(true);
        spike.position.x = col;
        spike.position.y = row;
      }
    }
  }

  return;
}

void Game::plant(float x, float y)
{
  if (spike.state == BombState::PLANTED)
  {
    return;
  }
  timePlanting = 0.0f;
  spike.position.x = x;
  spike.position.y = y;
  spike.state = BombState::PLANTED;

  return;
}

void Game::defuse()
{
  if (spike.state == BombState::DEFUSED)
  {
    return;
  }
  timeDefusing = 0.0f;
  spike.state = BombState::DEFUSED;

  return;
}

void Game::addDroppedWeapon(float x, float y, WeaponName weapon)
{
  for (const DroppedWeapon &dw : droppedWeapons)
  {
    if (dw.x == x && dw.y == y && dw.name == weapon)
    {
      return;
    }
  }
  droppedWeapons.emplace_back(DroppedWeapon{weapon, x, y});
}

void Game::stopPlantBomb(const std::string &name)
{
  findPlayerByName(name).updateIsPlanting(false);
  timePlanting = 0.0f;
}

void Game::stopDefuse(const std::string &name)
{
  findPlayerByName(name).updateIsDefusing(false);
  timeDefusing = 0.0f;
}

void Game::updatePlanting(const std::string &name, float deltaTime)
{
  timePlanting += deltaTime;
  Player &player = findPlayerByName(name);
  if (player.isPlanting() && timePlanting >= timeUntilPlant)
  {
    spike.state = BombState::PLANTED;
    player.setHasSpike(false);
  }
}

void Game::updateDefusing(const std::string &name, float deltaTime)
{
  timeDefusing += deltaTime;
  if (findPlayerByName(name).isDefusing() && timeDefusing >= timeUntilDefuse)
  {
    spike.state = BombState::DEFUSED;
  }
}

std::tuple<float, float, float, float> Game::getPlayerHitbox(const Player &player) const
{
  Hitbox hb = player.getHitbox();
  float x = player.getX();
  float y = player.getY();
  float width = hb.getWidth();
  float height = hb.getHeight();
  return {x, y, width, height};
}

PlayerCellBounds Game::getCellBounds(float x, float y, float width, float height) const
{

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

void Game::defuseBomb(const std::string &name)
{
  timeDefusing = 0.0f;
  Player &player = findPlayerByName(name);
  if (player.getRole() == Role::TERRORIST)
  { // solo puede defusear si es defensor
    return;
  }
  auto [x, y, width, height] = getPlayerHitbox(player);
  PlayerCellBounds bounds = getCellBounds(x, y, width, height);

  for (int row = bounds.topCell; row <= bounds.bottomCell; ++row)
  {
    for (int col = bounds.leftCell; col <= bounds.rightCell; ++col)
    {
      if (!map.isValidCell(row, col))
      {
        continue;
      }
      if (map.isSpikeSite(row, col))
      {
        player.updateIsDefusing(true);
        return;
      }
    }
  }
  return;
}

void Game::updatePlayerPosition(const std::string &name, float x, float y)
{
  findPlayerByName(name).setPosition(x, y);
}

void Game::updatePlayerHealth(const std::string &name, int health)
{
  Player &player = findPlayerByName(name);
  player.updateHealth(health - player.getHealth());
  if (!player.isAlive())
  {
    if (player.getHasTheSpike())
    {
      spike.state = BombState::DROPPED;
      spike.position.x = player.getX();
      spike.position.y = player.getY();
    }

    Weapon pw = player.getPrimaryWeapon();
    if (pw.name != WeaponName::NONE)
    {
      dropWeapon(pw, player.getX(), player.getY());
    }
    player.replaceWeapon(WeaponName::NONE);
    player.changeWeapon(WeaponType::SECONDARY);
  }
}

// Manuel: metodo creado para sincronizar el cliente
void Game::updatePrimaryWeapon(const std::string &name, WeaponName weapon)
{
  findPlayerByName(name).replaceWeapon(weapon);
}

void Game::updatePlayerMovement(Player &player, float deltaTime)
{
  Hitbox hb = player.getHitbox();

  std::pair<float, float> newPos = player.tryMove(deltaTime);
  float newX = newPos.first;
  float newY = newPos.second;
  float width = hb.getWidth();
  float height = hb.getHeight();
  float originalX = player.getX();
  float originalY = player.getY();
  PlayerCellBounds bounds = getCellBounds(newX, newY, width, height);
  for (const auto &other : players)
  {
    if (&other == &player)
      continue;
    if (!other.isAlive())
      continue;
    Hitbox otherHB = other.getHitbox();
    if (rectsOverlap(newX, newY, width, height, otherHB.x, otherHB.y, otherHB.width, otherHB.height))
    {
      return;
    }
  }

  if (!map.isColliding(bounds))
  {
    player.updateMovement(deltaTime, false, false);
    return;
  }
  bounds = getCellBounds(newX, originalY, width, height);
  if (!map.isColliding(bounds))
  {
    player.updateMovement(deltaTime, true, false);
    return;
  }
  bounds = getCellBounds(originalX, newY, width, height);
  if (!map.isColliding(bounds))
  {
    player.updateMovement(deltaTime, false, true);
    return;
  }
}

void Game::changeWeapon(const std::string &name, WeaponType type)
{
  findPlayerByName(name).changeWeapon(type);
}

void Game::buyWeapon(const std::string &name, WeaponName weaponName)
{
  Player &player = findPlayerByName(name);
  int price = Store::getWeaponPrice(weaponName);
  if (player.getMoney() >= price)
  {
    player.updateMoney(-price);
    player.replaceWeapon(weaponName);
    player.changeWeapon(WeaponType::PRIMARY);
    player.resetPrimaryBullets();
  }
}

std::vector<std::pair<WeaponName, int>> Game::getStore()
{
  return Store::getStore();
}

void Game::buyBullet(const std::string &name, WeaponType type)
{
  Player &player = findPlayerByName(name);
  if (player.getMoney() >= AMMO_PRICE)
  {
    if (type == WeaponType::PRIMARY)
    {
      player.resetPrimaryBullets();
    }
    else
    {
      player.resetSecondaryBullets();
    }
    player.updateMoney(-AMMO_PRICE);
  }
}

int Game::checkRoundWinner()
{
  if (teamA.getPlayersAlive() > 0 && teamB.getPlayersAlive() == 0)
  {
    teamA.incrementRoundsWon();
    return 1;
  }
  if (teamB.getPlayersAlive() > 0 && teamA.getPlayersAlive() == 0)
  {
    teamB.incrementRoundsWon();
    return 2;
  }
  return 0;
}

void Game::updateGamePhase(float deltaTime){
  switch (phase)
  {
  case Phase::PURCHASE:
    if(gameStart){
      teamA.resetSpikeCarrier();
      teamB.resetSpikeCarrier();
      gameStart=false;
    }
    purchaseElapsedTime += deltaTime;
    if (purchaseElapsedTime >= purchaseDuration)
    {
      purchaseElapsedTime = 0.0f;
      phase = Phase::BOMB_PLANTING;
      std::cout << "[DEBUG] Transition to BOMB_PLANTING phase" << std::endl;
    }
    break;

  case Phase::BOMB_PLANTING:
    plantingElapsedTime += deltaTime;
    if (checkRoundWinner() != 0 || plantingElapsedTime >= timeToPlantBomb)
    { // pierden atacantes
      std::cout << "[DEBUG] Ending BOMB_PLANTING, transitioning to PURCHASE" << std::endl;
      plantingElapsedTime = 0.0f;
      phase = Phase::PURCHASE;
      updateRounds();
    }
    if (spike.state == BombState::PLANTED)
    {
      phase = Phase::BOMB_DEFUSING;
      std::cout << "[DEBUG] Bomb planted, transitioning to BOMB_DEFUSING" << std::endl;
    }
    break;

  case Phase::BOMB_DEFUSING:
    bombElapsedTime += deltaTime;
    if (checkRoundWinner() != 0 || bombElapsedTime >= timeUntilBombExplode)
    { // pierden defensores
      std::cout << "[DEBUG] Ending BOMB_DEFUSING, transitioning to PURCHASE" << std::endl;
      bombElapsedTime = 0.0f;
      phase = Phase::PURCHASE;
      updateRounds();
    }
    if (spike.state == BombState::DEFUSED)
    {
      phase = Phase::PURCHASE;
      std::cout << "[DEBUG] Bomb defused, transitioning to PURCHASE" << std::endl;
      updateRounds();
    }

    break;

  default:
    break;
  }
}

void Game::updateRounds()
{
  teamA.restartPlayersAlive(); // contador en team y vida de c/u
  teamB.restartPlayersAlive();
  roundNumber += 1;
  roundsUntilRoleChange -= 1;
  roundsUntilEndGame -= 1;
  if (roundsUntilRoleChange == 0)
  {
    teamA.invertRole();
    teamB.invertRole();
  }
  if (roundsUntilEndGame == 0)
  {
    running = false;
  }
  resetSpawn();
  placeTeamsInSpawn();

  teamA.resetSpikeCarrier();
  teamB.resetSpikeCarrier();
  spike.state = BombState::INVENTORY;
  droppedWeapons.clear();
}

void Game::placeTeamsInSpawn()
{
  for (auto &player : players){
    placePlayerInSpawnTeam(player);
  }
}

StateGame Game::getState()
{
  StateGame state;
  state.phase = phase;

  std::vector<Entity> entities;

  for (const auto &player : players)
  {
    entities.push_back(getPlayerState(player.getName()));
  }
  Entity bomb;
  bomb.type = BOMB;
  BombData data;

  BombState bombState;
  bombState = spike.state;
  data.state = bombState;
  bomb.data = data;
  bomb.x = spike.position.x;
  bomb.y = spike.position.y;
  entities.push_back(bomb);

  for (const DroppedWeapon &dropped : droppedWeapons)
  {
    Entity weaponEntity;
    weaponEntity.type = EntityType::WEAPON;

    WeaponData weaponData;
    weaponData.weapon = dropped.name;

    weaponEntity.data = weaponData;
    weaponEntity.x = dropped.x;
    weaponEntity.y = dropped.y;

    entities.push_back(weaponEntity);
  }

  state.entities = entities;
  state.shots = shot_queue;

  return state;
}

Entity Game::getPlayerState(const std::string &name)
{
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
  data.name = player.getName();
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

void Game::makeShot(Player &shooter, const std::string &shooterName)
{
  int bullets = shooter.getBulletsPerShoot();
  if (shooter.getTypeEquipped() == WeaponType::PRIMARY)
  {
    shooter.updatePrimaryBullets(-bullets);
  }
  if (shooter.getTypeEquipped() == WeaponType::SECONDARY)
  {
    shooter.updateSecondaryBullets(-bullets);
  }
  Shot shot;

  for (int i = 0; i < bullets; i++)
  {
    auto [maxDistance, originX, originY, targetX, targetY, angle] =
        shooter.shoot();
    shot.origin_x = originX;
    shot.origin_y = originY;
    Bullet bullet;

    Player *closestPlayer = nullptr;
    float closestPlayerDist = maxDistance + 1.0f;
    std::pair<float, float> closestHitPoint;

    for (auto &player : players)
    {
      if (&player == &shooter)
        continue;
      if (player.getRole() == shooter.getRole())
      { // no permito q dañe a los de su equipo
        continue;
      }
      if (!player.isAlive())
      { // no permito q dispare a muertos
        continue;
      }

      Hitbox hb = player.getHitbox();
      auto hit_point = hb.intersectsRay(originX, originY, targetX, targetY);
      if (hit_point)
      {
        float dx = hit_point->first - originX;
        float dy = hit_point->second - originY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < closestPlayerDist)
        {
          closestPlayerDist = dist;
          closestPlayer = &player;
          closestHitPoint = *hit_point;
        }
      }
    }

    auto wallHit = rayHitsWall(originX, originY, targetX, targetY, maxDistance);

    float wallDist = maxDistance + 1.0f;
    std::pair<float, float> wallPoint;

    if (wallHit)
    {
      wallPoint = *wallHit;
      float dx = wallPoint.first - originX;
      float dy = wallPoint.second - originY;
      wallDist = std::sqrt(dx * dx + dy * dy);
    }
    bullet.angle = angle;
    if (closestPlayer && closestPlayerDist < wallDist)
    {
      applyDamageToPlayer(shooter, *closestPlayer, closestPlayerDist);
      bullet.target_x = closestHitPoint.first;
      bullet.target_y = closestHitPoint.second;
      bullet.impact = Impact::HUMAN;
    }
    else if (wallHit)
    {
      std::cout << shooterName << " disparó y la bala impactó una pared en ("
                << wallPoint.first << ", " << wallPoint.second << ")\n";
      bullet.target_x = wallPoint.first;
      bullet.target_y = wallPoint.second;
      bullet.impact = Impact::BLOCK;
    }
    else
    {
      bullet.target_x = targetX;
      bullet.target_y = targetY;
      bullet.impact = Impact::NOTHING;
    }
    shot.bullets.push_back(bullet);
  }
  shot.weapon = shooter.getEquipped().name;
  shot_queue.push(shot);
  shooter.setAlreadyShot(true);
}

void Game::applyDamageToPlayer(const Player &shooter, Player &target, float distance)
{
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
  if (!target.isAlive())
  {
    if (target.getHasTheSpike())
    {
      spike.state = BombState::DROPPED;
      spike.position.x = target.getX();
      spike.position.y = target.getY();
    }
    dropWeapon(target.getPrimaryWeapon(), target.getX(), target.getY());
  }
}

bool Game::rectsOverlap(float ax, float ay, float aw, float ah,
                        float bx, float by, float bw, float bh)
{
  return (ax < bx + bw) && (ax + aw > bx) &&
         (ay < by + bh) && (ay + ah > by);
}

void Game::grab(const std::string &name)
{
  Player &player = findPlayerByName(name);

  if (spike.state == BombState::DROPPED)
  {
    if (rectsOverlap(player.getX(), player.getY(), player.getHitbox().getWidth(), player.getHitbox().getHeight(),
                     spike.position.x, spike.position.y, BOMB_WIDTH, BOMB_HEIGHT))
    {
      player.setHasSpike(true);
      spike.state = BombState::INVENTORY;
      return;
    }
  }

  for (auto weapon = droppedWeapons.begin(); weapon != droppedWeapons.end(); ++weapon)
  {
    if (rectsOverlap(player.getX(), player.getY(), player.getHitbox().getWidth(), player.getHitbox().getHeight(),
                     weapon->x, weapon->y, WEAPON_WIDTH, WEAPON_HEIGHT))
    {

      droppedWeapons.erase(weapon);
      Weapon pw = player.getPrimaryWeapon();
      if (pw.name != WeaponName::NONE)
      {
        dropWeapon(pw, player.getX(), player.getY());
      }

      player.replaceWeapon(weapon->name);
      return;
    }
  }
}

void Game::dropWeapon(const Weapon &weapon, float x, float y)
{
  droppedWeapons.push_back({weapon.name, x, y});
}

void Game::shoot(const std::string &shooterName, float deltaTime)
{
  Player &shooter = findPlayerByName(shooterName);

  if (!shooter.isShooting())
  {
    return;
  }

  if (shooter.getShootCooldown() > 0)
  {
    return;
  }

  if (shooter.getBullets() < shooter.getBulletsPerShoot())
  {
    return;
  }

  const Weapon &equipped = shooter.getEquipped();

  if (equipped.burstFire)
  { // es arma con rafaga
    if (!shooter.getAlreadyShot() ||
        equipped.burstDelay <=
            shooter.getTimeLastBullet())
    { // puede disparar otra bala
      // aun quedan balas rafaga
      shooter.updateBurstFireBullets(-1);
      makeShot(shooter, shooterName);
      shooter.resetTimeLastBullet();
      if (shooter.getBurstFireBullets() == 0)
      { // fin rafaga
        shooter.resetCooldown();
        shooter.updateBurstFireBullets(equipped.bulletsPerBurst);
        shooter.updateTimeLastBullet(deltaTime);
      }
    }
    else
    {
      shooter.updateTimeLastBullet(deltaTime);
    }
  }
  else
  {
    if (!shooter.getAlreadyShot())
    {
      shooter.resetCooldown();
      makeShot(shooter, shooterName);
    }
  }
}

std::optional<std::pair<float, float>>
Game::rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  float length = std::sqrt(dx * dx + dy * dy);

  int steps = static_cast<int>(length * 50);
  float stepX = dx / steps;
  float stepY = dy / steps;

  float x = x0;
  float y = y0;

  for (int i = 0; i <= steps && i * std::hypot(stepX, stepY) <= maxDist; ++i)
  {
    int row = static_cast<int>(std::floor(y));
    int col = static_cast<int>(std::floor(x));

    if (!map.isValidCell(row, col))
    {
      break;
    }

    if (map.isBlocked(row, col))
    { // colisión con la pared
      return std::make_pair(x, y);
    }

    x += stepX;
    y += stepY;
  }

  return std::nullopt;
}

void Game::updateTime(float currentTime) { time = currentTime; }

void Game::updateRotation(const std::string &name, float currentRotation)
{
  findPlayerByName(name).setRotation(currentRotation);
}

float Game::getRotation(const std::string &name)
{
  return findPlayerByName(name).getRotation();
}

void Game::update(float deltaTime)
{
  for (auto &player : players)
  {
    updateGamePhase(deltaTime);
    updatePlayerMovement(player, deltaTime);
    player.updateCooldown(deltaTime);
    shoot(player.getName(), deltaTime);
    player.updateAceleration(deltaTime);
    updatePlanting(player.getName(), deltaTime);
    updateDefusing(player.getName(), deltaTime);
  }
}

void Game::execute(const std::string &name, Action action)
{

  switch (action.type)
  {
  case ActionType::MOVE:
  {
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->vx, data->vy, data->id);
    break;
  }
  case ActionType::POINT_TO:
  {
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);
    break;
  }

  case ActionType::SHOOT:
  {
    findPlayerByName(name).startShooting();
    break;
  }
  case ActionType::STOP_SHOOTING:
  { // en la de rafaga reiniciar rafaga
    stopShooting(name);
    break;
  }
  case ActionType::BUY_BULLET:
  {
    const BuyBulletAction *data = std::get_if<BuyBulletAction>(&action.data);
    buyBullet(name, data->type);
    break;
  }
  case ActionType::BUY_WEAPON:
  {
    const BuyWeaponAction *data = std::get_if<BuyWeaponAction>(&action.data);
    buyWeapon(name, data->weapon);
    break;
  }
  case ActionType::CHANGE_WEAPON:
  {
    const ChangeWeaponAction *data =
        std::get_if<ChangeWeaponAction>(&action.data);
    changeWeapon(name, data->type);
    break;
  }
  case ActionType::PLANT:
  {
    plantBomb(name);
    break;
  }
  case ActionType::STOP_PLANTING:
  {
    stopPlantBomb(name);
    break;
  }

  case ActionType::DEFUSE:
  {
    defuseBomb(name);
    break;
  }
  case ActionType::STOP_DEFUSING:
  {
    stopDefuse(name);
    break;
  }
  case ActionType::GRAB:
  {
    grab(name);
    break;
  }
  default:
  {

    break;
  }
  }
}

Shot Game::shotQueuePop()
{
  Shot top = shot_queue.front();
  shot_queue.pop();
  return top;
}

bool Game::shotQueueIsEmpty() { return shot_queue.empty(); }

void Game::shotQueueClear()
{
  std::queue<Shot> empty;
  std::swap(shot_queue, empty);
}

float Game::getX(const std::string &name)
{
  return findPlayerByName(name).getX();
}

float Game::getY(const std::string &name)
{
  return findPlayerByName(name).getY();
}