#include "game.h"

Game::Game(std::vector<std::vector<CellType>> game_map, GameRules &gameRules)
    : gameRules(gameRules), teamA(gameRules), teamB(gameRules),
      map(std::move(game_map)) {
  teamA.setRole(Role::COUNTER_TERRORIST);
  teamB.setRole(Role::TERRORIST);
  spawnTeamTerrorist = map.findSpawnTeam(true);
  spawnTeamCounter = map.findSpawnTeam(false);
  spike.state = BombState::INVENTORY;
  rounds.roundsWonTeamA = 0;
  rounds.roundsWonTeamB = 0;
  rounds.currentRound = 0;
  winner.team = '-';
  winner.typeEndRound = TypeEndRound::DEAD_TEAM;

  roundsUntilRoleChange = gameRules.rounds_until_role_change;
  roundsUntilEndGame = gameRules.rounds_until_end_game;
  timeUntilPlant = gameRules.time_until_plant;
  timeUntilDefuse = gameRules.time_until_defuse;
  timeUntilBombExplode = gameRules.bomb_duration;
  purchaseDuration = gameRules.purchase_duration;
  timeToPlantBomb = gameRules.time_to_plant;
  timeUntilNewRound = gameRules.time_until_new_round;
  timeUntilEndRunning= gameRules.timeUntilEndRunning;
}

bool Game::addPlayer(const std::string &name) {
  std::shared_ptr<Player> player = std::make_shared<Player>(name, gameRules);
  players.emplace_back(player);
  if (teamA.getTeamSize() < teamB.getTeamSize() &&
      teamA.getTeamSize() < gameRules.max_players_per_team) {
    teamA.addPlayer(player);
    player->role = teamA.getRole();
    placePlayerInSpawnTeam(*player);
    return true;
  }
  if (teamB.getTeamSize() < gameRules.max_players_per_team) {
    teamB.addPlayer(player);
    player->role = teamB.getRole();
    placePlayerInSpawnTeam(*player);
    return true;
  }
  return false;
}

void Game::placePlayerInSpawnTeam(Player &player) {
  std::vector<std::tuple<int, int, bool>> &spawn =
      (player.role == Role::COUNTER_TERRORIST) ? spawnTeamCounter
                                               : spawnTeamTerrorist;

  for (auto &pos : spawn) {
    int row = std::get<0>(pos);
    int col = std::get<1>(pos);
    bool used = std::get<2>(pos);

    if (!used) {
      float x = static_cast<float>(col) + 0.1f;
      float y = static_cast<float>(row) + 0.1f;

      std::get<2>(pos) = true;
      player.setPosition(x, y);

      return;
    }
  }
}
void Game::resetSpawn() {

  for (auto &pos : spawnTeamCounter) {
    std::get<2>(pos) = false;
  }
  for (auto &pos : spawnTeamTerrorist) {
    std::get<2>(pos) = false;
  }
}

Player& Game::findPlayerByName(const std::string& name) {
    auto it = std::find_if(players.begin(), players.end(),
        [&name](const std::shared_ptr<Player>& player) {
            return player->name == name;
        });
    
    if (it != players.end()) {
        return **it;
    }

    throw std::runtime_error("Player not found");
}

void Game::stopShooting(const std::string &name) {
  Player &player = findPlayerByName(name);
  player.alreadyShot = false;
  player.shooting = false;
}

void Game::movePlayer(const std::string &name, float vx, float vy) {
  Player &player = findPlayerByName(name);
  player.updateVelocity(vx, vy);
}

void Game::plantBomb(const std::string &name) {
  timePlanting = 0.0f;
  const Player &player = findPlayerByName(name);
  if (!player.hasTheSpike) {
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
        findPlayerByName(name).planting = true;
        spike.position.x = col;
        spike.position.y = row;
      }
    }
  }

  return;
}

void Game::stopPlantBomb(const std::string &name) {
  findPlayerByName(name).planting = false;
  timePlanting = 0.0f;
}

void Game::stopDefuse(const std::string &name) {
  findPlayerByName(name).defusing = false;
  timeDefusing = 0.0f;
}

void Game::updatePlanting(const std::string &name, float deltaTime) {
  timePlanting += deltaTime;
  Player &player = findPlayerByName(name);
  if (player.planting && timePlanting >= timeUntilPlant) {
    spike.state = BombState::PLANTED;
    player.hasTheSpike = false;
  }
}

void Game::updateDefusing(const std::string &name, float deltaTime) {
  timeDefusing += deltaTime;
  if (findPlayerByName(name).defusing && timeDefusing >= timeUntilDefuse) {
    spike.state = BombState::DEFUSED;
  }
}

std::tuple<float, float, float, float>
Game::getPlayerHitbox(const Player &player) {
  Hitbox hb = player.hitbox;
  float x = player.x;
  float y = player.y;
  float width = hb.getWidth();
  float height = hb.getHeight();
  return {x, y, width, height};
}

PlayerCellBounds Game::getCellBounds(float x, float y, float width,
                                     float height) {

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
  timeDefusing = 0.0f;
  Player &player = findPlayerByName(name);
  if (player.role == Role::TERRORIST) {
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
        player.defusing = true;
        return;
      }
    }
  }
  return;
}

void Game::updatePlayerMovement(Player &player, float deltaTime) const {
  Hitbox hb = player.hitbox;

  std::pair<float, float> newPos = player.tryMove(deltaTime);
  float newX = newPos.first;
  float newY = newPos.second;
  float width = hb.getWidth();
  float height = hb.getHeight();
  float originalX = player.x;
  float originalY = player.y;
  PlayerCellBounds bounds = getCellBounds(newX, newY, width, height);
  for (const auto &other_ptr : players) {
    Player &other = *other_ptr;
    if (&other == &player)
      continue;
    if (!other.alive)
      continue;
    Hitbox otherHB = other.hitbox;
    if (rectsOverlap(newX, newY, width, height, otherHB.x, otherHB.y,
                     otherHB.width, otherHB.height)) {
      return;
    }
  }

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

int Game::getWeaponPrice(WeaponName name) {
    auto it = gameRules.weapons.find(name);
    if (it != gameRules.weapons.end()) {
        return it->second.price;
    }
    return -1;
}

void Game::buyWeapon(const std::string &name, WeaponName weaponName) {
  Player &player = findPlayerByName(name);
  int price = getWeaponPrice(weaponName);
  if (player.money >= price) {
    player.updateMoney(-price);
    player.replaceWeapon(weaponName);
    player.changeWeapon(WeaponType::PRIMARY);
    player.resetPrimaryBullets();
  }
}

void Game::buyBullet(const std::string &name, WeaponType type) {
  Player &player = findPlayerByName(name);
  if (player.money >= gameRules.ammo_price) {
    if (type == WeaponType::PRIMARY) {
      player.resetPrimaryBullets();
    } else {
      player.resetSecondaryBullets();
    }
    player.updateMoney(-gameRules.ammo_price);
  }
}

char Game::checkRoundWinner() const {
  if (teamA.getPlayersAlive() > 0 && teamB.getPlayersAlive() == 0) {
    return 'a';
  }
  if (teamB.getPlayersAlive() > 0 && teamA.getPlayersAlive() == 0) {
    return 'b';
  }
  return '-';
}

void Game::placeTeamsInSpawn() {
  for (auto &player_ptr : players) {
    Player &player = *player_ptr;
    placePlayerInSpawnTeam(player);
  }
}

bool Game::isRunning() const { return running; }

void Game::stop() { running = false; }

void Game::makeShot(Player &shooter) {
  int bullets = shooter.getBulletsPerShoot();
  subtractAmmo(shooter, bullets);
  Shot shot;
  
  for (int i = 0; i < bullets; i++) {
    shot.origin_x = shooter.hitbox.x + shooter.hitbox.width / 2.0f;;
    shot.origin_y = shooter.hitbox.y + shooter.hitbox.height / 2.0f;;
    shot.bullets.push_back(simulateBullet(shooter));
  }
  shot.weapon = shooter.equipped.name;
  shot_queue.push(shot);
  shooter.alreadyShot = true;
}

Bullet Game::simulateBullet(Player &shooter) {
  auto [maxDistance, originX, originY, targetX, targetY, angle] = shooter.shoot();

  Bullet bullet;
  bullet.angle = angle;

  Player *closestPlayer = nullptr;
  float closestPlayerDist = maxDistance + 1.0f;
  std::pair<float, float> closestHitPoint;

  for (auto &player_ptr : players) {
    Player &player = *player_ptr;
    if (&player == &shooter || player.role == shooter.role || !player.alive)
      continue;

    if (auto hit_point = player.hitbox.intersectsRay(originX, originY, targetX, targetY)) {
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
    applyDamageToPlayer(shooter, *closestPlayer, closestPlayerDist);
    bullet.target_x = closestHitPoint.first;
    bullet.target_y = closestHitPoint.second;
    bullet.impact = Impact::HUMAN;
  } else if (wallHit) {
    bullet.target_x = wallPoint.first;
    bullet.target_y = wallPoint.second;
    bullet.impact = Impact::BLOCK;
  } else {
    bullet.target_x = targetX;
    bullet.target_y = targetY;
    bullet.impact = Impact::NOTHING;
  }

  return bullet;
}


void Game::applyDamageToPlayer(const Player &shooter, Player &target,
                               float distance) {
  std::pair<float, float> damageRange = shooter.getDamageRange();

  float baseDamage = 1000.0f / (distance + 1.0f);
  float clampedDamage =
      std::clamp(baseDamage, damageRange.first, damageRange.second);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(damageRange.first, damageRange.second);
  float randomDamage = dis(gen);

  float finalDamage = std::min(clampedDamage, randomDamage);
  int damageToApply  = static_cast<int>(std::ceil(finalDamage));

  target.updateHealth(-damageToApply);
  if (!target.alive) {
    handlePlayerDeath(target);
  }
}

void Game::subtractAmmo(Player &shooter, int bullets) {
  switch (shooter.typeEquipped) {
    case WeaponType::PRIMARY:
      shooter.updatePrimaryBullets(-bullets);
      break;
    case WeaponType::SECONDARY:
      shooter.updateSecondaryBullets(-bullets);
      break;
    default:
      break;
  }
}

void Game::handlePlayerDeath(Player &target) {
  if (target.hasTheSpike) {
    spike.state = BombState::DROPPED;
    target.hasTheSpike = false;
    spike.position = {target.x, target.y};
  }

  dropWeapon(target.primaryWeapon, target.x, target.y);
  target.replaceWeapon(WeaponName::NONE);
  target.changeWeapon(WeaponType::SECONDARY);
}

bool Game::rectsOverlap(float ax, float ay, float aw, float ah, float bx,
                        float by, float bw, float bh) {
  return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
}

void Game::grab(const std::string &name) {
  Player &player = findPlayerByName(name);

  if (spike.state == BombState::DROPPED && player.role==Role::TERRORIST) {
    if (rectsOverlap(player.x, player.y, player.hitbox.getWidth(),
                     player.hitbox.getHeight(), spike.position.x,
                     spike.position.y, BOMB_WIDTH, BOMB_HEIGHT)) {
      player.hasTheSpike = true;
      spike.state = BombState::INVENTORY;
      return;
    }
  }

  for (auto weapon = droppedWeapons.begin(); weapon != droppedWeapons.end(); ++weapon) {
  if (rectsOverlap(player.x, player.y, player.hitbox.getWidth(),
                   player.hitbox.getHeight(), weapon->x, weapon->y,
                   WEAPON_WIDTH, WEAPON_HEIGHT)) {
    WeaponName newWeaponName = weapon->name;
    droppedWeapons.erase(weapon);

    if (player.primaryWeapon.name != WeaponName::NONE) {
      dropWeapon(player.primaryWeapon, player.x, player.y);
    }

    player.replaceWeapon(newWeaponName);
    player.changeWeapon(WeaponType::PRIMARY);
    return;
  }
}

}

void Game::dropWeapon(const Weapon &weapon, float x, float y) {
  if(weapon.name!=WeaponName::NONE){
    droppedWeapons.push_back({weapon.name, x, y});
  }
}

void Game::shoot(const std::string &shooterName, float deltaTime) {
  Player &shooter = findPlayerByName(shooterName);

  if (!shooter.shooting) return;

  if (shooter.shootCooldown > 0) return;

  if (shooter.getBullets() < shooter.getBulletsPerShoot()) return;

  const Weapon &equipped = shooter.equipped;

  if (equipped.burstFire) {
    if (!shooter.alreadyShot || equipped.burstDelay <= shooter.timeLastBullet) {
      shooter.updateBurstFireBullets(-1);
      makeShot(shooter);
      shooter.resetTimeLastBullet();
      if (shooter.burstFireBullets == 0) {
        shooter.resetCooldown();
        shooter.updateBurstFireBullets(equipped.bulletsPerBurst);
        shooter.updateTimeLastBullet(deltaTime);
      }
    } else {
      shooter.updateTimeLastBullet(deltaTime);
    }
  } else {
    if (!shooter.alreadyShot) {
      shooter.resetCooldown();
      makeShot(shooter);
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

    if (!map.isValidCell(row, col)) {
      break;
    }

    if (map.isBlocked(row, col)) {
      return std::make_pair(x, y);
    }

    x += stepX;
    y += stepY;
  }

  return std::nullopt;
}

void Game::updateRotation(const std::string &name, float currentRotation) {
  findPlayerByName(name).rotation = currentRotation;
}

StateGame Game::getState() {
  StateGame state;
  state.phase = phase;

  std::vector<Entity> entities;

  for (const auto &player_ptr : players) {
    Player &player = *player_ptr;
    entities.push_back(getPlayerState(player.name));
  }

  entities.push_back(getBombState());

  std::transform(droppedWeapons.begin(), droppedWeapons.end(),
                 std::back_inserter(entities),
                 [this](const DroppedWeapon &dropped) {
                     return getDroppedWeaponState(dropped);
                 });

  state.entities = entities;
  state.shots = shot_queue;
  state.rounds = rounds;

  return state;
}

Entity Game::getBombState() const {
  Entity bomb;
  bomb.type = BOMB;
  BombData data;

  BombState bombState;
  bombState = spike.state;
  data.state = bombState;
  bomb.data = data;
  bomb.x = spike.position.x;
  bomb.y = spike.position.y;
  return bomb;
}

Entity Game::getDroppedWeaponState(const DroppedWeapon &dw){
  Entity weaponEntity;
  weaponEntity.type = EntityType::WEAPON;

  WeaponData weaponData;
  weaponData.weapon = dw.name;

  weaponEntity.data = weaponData;
  weaponEntity.x = dw.x;
  weaponEntity.y = dw.y;
  return weaponEntity;
}


Entity Game::getPlayerState(const std::string &name) {
  Player player = findPlayerByName(name);
  Entity entity;
  entity.type = PLAYER;
  entity.x = player.x;
  entity.y = player.y;
  Inventory inv;
  inv.primary = player.primaryWeapon.name;
  inv.secondary = player.secondaryWeapon.name;
  inv.bulletsPrimary = player.bulletsPrimary;
  inv.bulletsSecondary = player.bulletsSecondary;
  inv.has_the_bomb = player.hasTheSpike;
  PlayerData data;
  data.equippedWeapon = player.typeEquipped;
  data.inventory = inv;
  data.name = player.name;
  data.rotation = player.rotation;
  data.health = player.health;
  data.money = player.money;
  data.alive = player.alive;
  data.terrorist = (player.role == Role::TERRORIST);
  entity.data = data;
  return entity;
}

void Game::handleEndRound(char winnerTeam, TypeEndRound type) {
  elapsedTime=0.0f;
  winner.team = winnerTeam;
  winner.typeEndRound = type;
  rounds.winner = winner;

  if (winnerTeam == 'a') {
    teamA.updateMoneyAfterRound(gameRules.money_winner);
    teamB.updateMoneyAfterRound(gameRules.money_loser);
    rounds.roundsWonTeamA++;
  } else {
    teamA.updateMoneyAfterRound(gameRules.money_loser);
    teamB.updateMoneyAfterRound(gameRules.money_winner);
    rounds.roundsWonTeamB++;
  }

  rounds.currentRound++;
  phase = Phase::END_ROUND;
}

void Game::updateGamePhase(float deltaTime) {
  char roundWinner = '-';
  elapsedTime += deltaTime;
  switch (phase) {
  case Phase::PURCHASE:
    if (gameStart) {
      teamA.resetSpikeCarrier();
      teamB.resetSpikeCarrier();
      gameStart = false;
    }
    if (elapsedTime >= purchaseDuration) {
      elapsedTime=0.0f;
      phase = Phase::BOMB_PLANTING;
    }
    break;

  case Phase::BOMB_PLANTING:
    roundWinner = checkRoundWinner();

    if (roundWinner != '-') {
      handleEndRound(roundWinner, TypeEndRound::DEAD_TEAM);
    } else if (elapsedTime >= timeToPlantBomb) {
      char winningTeam =
          (teamA.getRole() == Role::COUNTER_TERRORIST) ? 'a' : 'b';
      handleEndRound(winningTeam, TypeEndRound::BOMB_NOT_PLANTED);
    } else if (spike.state == BombState::PLANTED) {
      elapsedTime=0.0f;
      phase = Phase::BOMB_DEFUSING;
    }
    break;

  case Phase::BOMB_DEFUSING:
    roundWinner = checkRoundWinner();
    if ((roundWinner == 'a' && teamA.getRole() == Role::TERRORIST) ||
          (roundWinner == 'b' && teamB.getRole() == Role::TERRORIST)) {
    
      handleEndRound(roundWinner, TypeEndRound::DEAD_TEAM);

    } else if (elapsedTime >= timeUntilBombExplode) {
      char winningTeam = (teamA.getRole() == Role::TERRORIST) ? 'a' : 'b';
      handleEndRound(winningTeam, TypeEndRound::BOMB_EXPLODED);

    } else if (spike.state == BombState::DEFUSED) {
      char winningTeam =
          (teamA.getRole() == Role::COUNTER_TERRORIST) ? 'a' : 'b';
      handleEndRound(winningTeam, TypeEndRound::BOMB_DEFUSED);
    }
    break;

  case Phase::END_ROUND:
    
    if (elapsedTime >= timeUntilNewRound) {
      elapsedTime=0.0f;
      phase = Phase::PURCHASE;
      updateRounds();
      if(endGame){
        phase = Phase::END_GAME;
      }
    }
    break;
  case Phase::END_GAME:
    if (elapsedTime >= timeUntilEndRunning) {
      elapsedTime=0.0f;
      running = false;
    }
    break;

  default:
    break;
  }
}

void Game::updateRounds() {
  elapsedTime = 0.0f;
  roundNumber += 1;
  roundsUntilRoleChange -= 1;
  roundsUntilEndGame -= 1;
  if (roundsUntilEndGame == 0) {
    endGame=true;
    return;
  }

  teamA.restartPlayersAlive();
  teamB.restartPlayersAlive();
  
  if (roundsUntilRoleChange == 0) {
    teamA.invertRole();
    teamB.invertRole();
  }
  
  resetSpawn();
  placeTeamsInSpawn();

  teamA.resetSpikeCarrier();
  teamB.resetSpikeCarrier();

  spike.state = BombState::INVENTORY;
  droppedWeapons.clear();
}

void Game::update(float deltaTime) {
  updateGamePhase(deltaTime);
  updatePlayers(deltaTime);
}

void Game::updatePlayers(float deltaTime){
  for (auto &player_ptr : players) {
    Player &player = *player_ptr;
    updatePlayerMovement(player, deltaTime);
    player.updateCooldown(deltaTime);
    shoot(player.name, deltaTime);
    player.updateAceleration(deltaTime);
    updatePlanting(player.name, deltaTime);
    updateDefusing(player.name, deltaTime);
  }
}

void Game::execute(const std::string &name, Action action) {

  switch (action.type) {
  case ActionType::MOVE: {
    const MoveAction *data = std::get_if<MoveAction>(&action.data);
    movePlayer(name, data->vx, data->vy);
    break;
  }
  case ActionType::POINT_TO: {
    const PointToAction *pointToData = std::get_if<PointToAction>(&action.data);
    updateRotation(name, pointToData->value);
    break;
  }

  case ActionType::SHOOT: {
    findPlayerByName(name).shooting = true;
    break;
  }
  case ActionType::STOP_SHOOTING: {
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
  case ActionType::GRAB: {
    grab(name);
    break;
  }
  default: {
    break;
  }
  }
}

void Game::shotQueueClear()
{
  std::queue<Shot> empty;
  std::swap(shot_queue, empty);
}

