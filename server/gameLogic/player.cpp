#include "player.h"
#include <iostream>

Player::Player(const std::string &name, GameRules &gameRules)
    : gameRules(gameRules),
      name(name),
      x(0),
      y(0),
      hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},
      role(Role::COUNTER_TERRORIST),
      money(gameRules.initial_money),
      equipped(gameRules.weapons.at(WeaponName::GLOCK)),  // Inicializo con alguna arma válida (por ejemplo secondaryWeapon)
      typeEquipped(WeaponType::SECONDARY),
      knife(gameRules.weapons.at(WeaponName::KNIFE)),
      primaryWeapon(gameRules.weapons.at(WeaponName::NONE)),
      secondaryWeapon(gameRules.weapons.at(WeaponName::GLOCK)),
      bulletsPrimary(gameRules.initial_primary_ammo),
      bulletsSecondary(gameRules.initial_secondary_ammo),
      rotation(0),
      health(gameRules.max_health),
      vx(0),
      vy(0),
      aceleration(0),
      hasTheSpike(true),
      shooting(false),
      planting(false),
      defusing(false),
      alreadyShot(false),
      shootCooldown(0.0f),
      timeLastBullet(0.0f),
      burstFireBullets(1),
      lastVx(0.0f),
      lastVy(0.0f),
      slideTimer(0.0f),
      alive(true)
{}


void Player::move(float deltaTime, bool onlyX, bool onlyY) {
  float totalSpeed = gameRules.speed + aceleration;
  float dx = 0.0f;
  float dy = 0.0f;

  if (vx != 0.0f || vy != 0.0f) {
    dx = vx;
    dy = vy;
  } else if (slideTimer > 0.0f) {
    dx = lastVx;
    dy = lastVy;
    slideTimer -= deltaTime;
  }

  if (onlyX) {
    this->x += dx * deltaTime * totalSpeed;
  } else if (onlyY) {
    this->y += dy * deltaTime * totalSpeed;
  } else {
    this->x += dx * deltaTime * totalSpeed;
    this->y += dy * deltaTime * totalSpeed;
  }

  hitbox.x = this->x;
  hitbox.y = this->y;
}

std::pair<float, float> Player::tryMove(float deltaTime) const {
  float dirX =
      (vx != 0.0f || vy != 0.0f) ? vx : ((slideTimer > 0.0f) ? lastVx : 0.0f);
  float dirY =
      (vx != 0.0f || vy != 0.0f) ? vy : ((slideTimer > 0.0f) ? lastVy : 0.0f);

  if (dirX == 0.0f && dirY == 0.0f) {
    return {x, y};
  }

  float magnitude = std::sqrt(dirX * dirX + dirY * dirY);
  dirX /= magnitude;
  dirY /= magnitude;

  float boost = 0.1f;
  float baseSpeed = gameRules.speed + boost;
  float totalSpeed = baseSpeed + aceleration;

  float newX = x + dirX * deltaTime * totalSpeed;
  float newY = y + dirY * deltaTime * totalSpeed;

  return {newX, newY};
}

void Player::setPosition(float x, float y) {
  this->x = x;
  this->y = y;
  hitbox.x = x;
  hitbox.y = y;
}

void Player::updateHealth(int value) {
  health += value;
  if (health <= 0) {
    health = 0;
    alive = false;
  } else {
    alive = true;
  }
}

void Player::restoreHealth() { health = gameRules.max_health; }

void Player::updateMovement(float deltaTime, bool onlyX, bool onlyY) {
  move(deltaTime, onlyX, onlyY);
}

void Player::updateVelocity(float newVx, float newVy) {
  if (newVx != 0.0f || newVy != 0.0f) {
    if (newVx != 0 && newVy != 0) {
      newVx /= std::sqrt(2);
      newVy /= std::sqrt(2);
    }
    lastVx = newVx;
    lastVy = newVy;
    slideTimer = SLIDE_DURATION;
  }

  vx = newVx;
  vy = newVy;
}

void Player::updateAceleration(float deltaTime) {
  if (vx != 0.0f || vy != 0.0f) {
    aceleration += ACELERATION_RATE * deltaTime;
  } else {
    aceleration = MIN_ACELERATION;
  }

  if (aceleration < MIN_ACELERATION)
    aceleration = MIN_ACELERATION;
  if (aceleration > MAX_ACELERATION)
    aceleration = MAX_ACELERATION;
}

void Player::updateCooldown(float deltaTime) {
  if (shootCooldown > 0.0f) {
    shootCooldown -= deltaTime;
  }
}

void Player::resetCooldown() { shootCooldown = equipped.cooldown; }
std::pair<float, float> Player::getDamageRange() const {
  return std::make_pair(equipped.minDamage, equipped.maxDamage);
}

std::tuple<float, float, float, float, float, float> Player::shoot() {
  timeLastBullet = 0.0f;

  float origin_x = hitbox.x + hitbox.width / 2.0f;
  float origin_y = hitbox.y + hitbox.height / 2.0f;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(rotation - getSpreadAngle(),
                                             rotation + getSpreadAngle());
  float angle = dist(gen);
  float angle_rad = angle * M_PI / 180.0f;
  float max_distance;

  max_distance = equipped.maxRange;
  float target_x = origin_x + cos(angle_rad) * max_distance;
  float target_y = origin_y + sin(angle_rad) * max_distance;

  return std::make_tuple(max_distance, origin_x, origin_y, target_x, target_y,
                         angle);
}

int Player::getBulletsPerShoot() const { return equipped.bulletsPerShoot; }

float Player::getSpreadAngle() const { return equipped.spreadAngle; }

void Player::changeWeapon(WeaponType newEquippedWeapon) {
  if (newEquippedWeapon == WeaponType::PRIMARY) {
    equipped = primaryWeapon;
  } else if (newEquippedWeapon == WeaponType::SECONDARY) {
    equipped = secondaryWeapon;
  } else {
    equipped = knife;
  }
  typeEquipped = newEquippedWeapon;
}

void Player::replaceWeapon(WeaponName weapon) {
  primaryWeapon = gameRules.weapons[weapon];
}

void Player::updateMoney(int value) { money += value; }

void Player::updatePrimaryBullets(int value) { bulletsPrimary += value; }
void Player::updateSecondaryBullets(int value) { bulletsSecondary += value; }

void Player::resetPrimaryBullets() { bulletsPrimary = primaryWeapon.maxAmmo; }
void Player::resetSecondaryBullets() {
  bulletsSecondary = secondaryWeapon.maxAmmo;
}

void Player::updateTimeLastBullet(float deltaTime) {
  timeLastBullet += deltaTime;
}

void Player::resetTimeLastBullet() { timeLastBullet = 0; }

void Player::updateBurstFireBullets(int value) { burstFireBullets += value; }

int Player::getBullets() const {
  if (typeEquipped == WeaponType::PRIMARY) {
    return bulletsPrimary;
  } else if (typeEquipped == WeaponType::SECONDARY) {
    return bulletsSecondary;
  } else {
    return 1;
  }
}