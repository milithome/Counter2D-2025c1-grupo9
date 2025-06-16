#include "game.h"

Game::Game(std::vector<std::vector<CellType>> game_map)
    : map(std::move(game_map))
{
  teamA.setRole(Role::COUNTER_TERRORIST);
  teamB.setRole(Role::TERRORIST);
  spawnTeamTerrorist = map.findSpawnTeam(false); // true para terrorist
  spawnTeamCounter = map.findSpawnTeam(true);
  spike.state = BombState::INVENTORY;
  rounds.roundsWonTeamA=0;
  rounds.roundsWonTeamB=0;
  rounds.currentRound=0;
  winner.team='-';
  winner.typeEndRound=TypeEndRound::DEAD_TEAM;
}

bool Game::addPlayer(const std::string &name)
{
  std::shared_ptr<Player> player = std::make_shared<Player>(name);
  players.emplace_back(player);
  //Player &player = findPlayerByName(name);
  if (teamA.getTeamSize() < teamB.getTeamSize() && teamA.getTeamSize() < MAX_PLAYERS_PER_TEAM)
  {
    teamA.addPlayer(player);
    player->setRole(teamA.getRole());
    placePlayerInSpawnTeam(*player);
    return true;
  }
  if (teamB.getTeamSize() < MAX_PLAYERS_PER_TEAM)
  {
    teamB.addPlayer(player);
    player->setRole(teamB.getRole());
    placePlayerInSpawnTeam(*player);
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
    if (player->getName() == name)
      return *player;
  }
  throw std::runtime_error("Player not found");
}

void Game::stopShooting(const std::string &name)
{
  Player &player = findPlayerByName(name);
  player.setAlreadyShot(false);
  player.stopShooting();
}

void Game::movePlayer(const std::string &name, float vx, float vy)
{
  Player &player = findPlayerByName(name);
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
  for (const auto &other_ptr : players)
  {
    Player &other = *other_ptr;
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

char Game::checkRoundWinner(){
  if (teamA.getPlayersAlive() > 0 && teamB.getPlayersAlive() == 0){
    teamA.incrementRoundsWon();
    return 'a';
  }
  if (teamB.getPlayersAlive() > 0 && teamA.getPlayersAlive() == 0){
    teamB.incrementRoundsWon();
    return 'b';
  }
  return '-';
}

void Game::placeTeamsInSpawn()
{
  for (auto &player_ptr : players){
    Player &player = *player_ptr;
    placePlayerInSpawnTeam(player);
  }
}


bool Game::isRunning() { return running; }

void Game::stop() { running = false; }

void Game::makeShot(Player &shooter) {
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

    for (auto &player_ptr : players)
    {
      Player &player = *player_ptr;
      if (&player == &shooter)
        continue;
      if (player.getRole() == shooter.getRole())
      {
        continue;
      }
      if (!player.isAlive())
      {
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
      bullet.target_x=closestHitPoint.first;
      bullet.target_y=closestHitPoint.second;
      bullet.impact=Impact::HUMAN;

    } else if (wallHit) {
      bullet.target_x=wallPoint.first;
      bullet.target_y=wallPoint.second;
      bullet.impact=Impact::BLOCK;
    } else {
      bullet.target_x=targetX;
      bullet.target_y=targetY;
      bullet.impact=Impact::NOTHING;
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
      target.setHasSpike(false);
      spike.position.x = target.getX();
      spike.position.y = target.getY();
    }
    dropWeapon(target.getPrimaryWeapon(), target.getX(), target.getY());
    target.replaceWeapon(WeaponName::NONE); 
    target.changeWeapon(WeaponType::SECONDARY);
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
        player.replaceWeapon(WeaponName::NONE);
        player.changeWeapon(WeaponType::SECONDARY);
      }

      player.replaceWeapon(weapon->name);
      player.changeWeapon(WeaponType::PRIMARY);
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
  {
    if (!shooter.getAlreadyShot() ||
        equipped.burstDelay <=
            shooter.getTimeLastBullet())
    { 
      shooter.updateBurstFireBullets(-1);
      makeShot(shooter);
      shooter.resetTimeLastBullet();
      if (shooter.getBurstFireBullets() == 0)
      {
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
      makeShot(shooter);
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
    {
      return std::make_pair(x, y);
    }

    x += stepX;
    y += stepY;
  }

  return std::nullopt;
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

void Game::updateRotation(const std::string &name, float currentRotation)
{
  findPlayerByName(name).setRotation(currentRotation);
}


StateGame Game::getState()
{
  StateGame state;
  state.phase = phase;

  std::vector<Entity> entities;

  for (const auto &player_ptr : players)
  {
    Player &player = *player_ptr;
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
  state.rounds=rounds;

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
  data.health = player.getHealth();
  data.money = player.getMoney();
  data.alive = player.isAlive();
  entity.data = data;
  return entity;
}

void Game::handleEndRound(char winnerTeam, TypeEndRound type) {
    winner.team = winnerTeam;
    winner.typeEndRound = type;
    rounds.winner = winner;

    if (winnerTeam == 'a') {
        teamA.updateMoneyAfterRound(MONEY_WINNER);
        teamB.updateMoneyAfterRound(MONEY_LOSER);
        rounds.roundsWonTeamA++;
    } else {
        teamA.updateMoneyAfterRound(MONEY_LOSER);
        teamB.updateMoneyAfterRound(MONEY_WINNER);
        rounds.roundsWonTeamB++;
    }

    rounds.currentRound++;
    phase = Phase::END_ROUND;
}


void Game::updateGamePhase(float deltaTime) {
    char roundWinner = '-';

    switch (phase) {
    case Phase::PURCHASE:
        if (gameStart) {
            teamA.resetSpikeCarrier();
            teamB.resetSpikeCarrier();
            gameStart = false;
        }

        purchaseElapsedTime += deltaTime;
        if (purchaseElapsedTime >= purchaseDuration) {
            phase = Phase::BOMB_PLANTING;
        }
        break;

    case Phase::BOMB_PLANTING:
        plantingElapsedTime += deltaTime;
        roundWinner = checkRoundWinner();

        if (roundWinner != '-') {
            handleEndRound(roundWinner, TypeEndRound::DEAD_TEAM);
        } else if (plantingElapsedTime >= timeToPlantBomb) {
            char winningTeam = (teamA.getRole() == Role::COUNTER_TERRORIST) ? 'a' : 'b';
            handleEndRound(winningTeam, TypeEndRound::BOMB_NOT_PLANTED);
        } else if (spike.state == BombState::PLANTED) {
            phase = Phase::BOMB_DEFUSING;
        }
        break;

    case Phase::BOMB_DEFUSING:
        bombElapsedTime += deltaTime;
        roundWinner = checkRoundWinner();

        if (roundWinner != '-') {
            Role roleA = teamA.getRole();
            Role roleB = teamB.getRole();

            if ((roundWinner == 'a' && roleA == Role::TERRORIST) ||
                (roundWinner == 'b' && roleB == Role::TERRORIST)) {
                handleEndRound(roundWinner, TypeEndRound::DEAD_TEAM);
            }

        } else if (bombElapsedTime >= timeUntilBombExplode) {
            char winningTeam = (teamA.getRole() == Role::TERRORIST) ? 'a' : 'b';
            handleEndRound(winningTeam, TypeEndRound::BOMB_EXPLODED);

        } else if (spike.state == BombState::DEFUSED) {
            char winningTeam = (teamA.getRole() == Role::COUNTER_TERRORIST) ? 'a' : 'b';
            handleEndRound(winningTeam, TypeEndRound::BOMB_DEFUSED);
        }
        break;

    case Phase::END_ROUND:
        endRoundElapsedTime += deltaTime;
        if (endRoundElapsedTime >= timeUntilNewRound) {

            phase = Phase::PURCHASE;
            updateRounds();
        }
        break;

    default:
        break;
    }
}


void Game::updateRounds(){
  teamA.restartPlayersAlive();
  teamB.restartPlayersAlive();
  purchaseElapsedTime=0.0f;
  endRoundElapsedTime=0.0f;
  plantingElapsedTime = 0.0f;
  bombElapsedTime = 0.0f;
  
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

void Game::update(float deltaTime)
{
  for (auto &player_ptr : players)
  {
    Player &player = *player_ptr;
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
    movePlayer(name, data->vx, data->vy);
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
  {
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

