#include "player.h"
#include <iostream>

std::string Player::getName() const { return name; }

void Player::setRole(Role new_role) { role = new_role; }

float Player::getX() const { return x; }

float Player::getY() const { return y; }

void Player::move(float deltaTime, bool onlyX, bool onlyY) {
  float totalSpeed = SPEED + aceleration;
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

std::pair<float, float> Player::tryMove(float deltaTime) {
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
  float baseSpeed = SPEED + boost;
  float totalSpeed = baseSpeed + aceleration;

  float newX = x + dirX * deltaTime * totalSpeed;
  float newY = y + dirY * deltaTime * totalSpeed;

  return {newX, newY};
}

void Player::setPosition(float x, float y) {
  // coordenadas literales en las que quiero que se encuentre
  this->x = x;
  this->y = y;
  hitbox.x = x;
  hitbox.y = y;
}

float Player::getRotation() const { return rotation; }

void Player::setRotation(float currentRotation) { rotation = currentRotation; }

const Hitbox &Player::getHitbox() const { return hitbox; }

void Player::updateHealth(float value) {
  health += value;
  if (health < 0.0f)
    health = 0.0f;
}

void Player::setHealth(float value) {
  health = value;
  if (health < 0.0f)
    health = 0.0f;
}

void Player::updateMovement(float deltaTime, bool onlyX, bool onlyY) {
  move(deltaTime, onlyX, onlyY);
}

float Player::getHealth() const { return health; }

bool Player::isAlive() const { return health > 0.0f; }

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

void Player::stopShooting() { shooting = false; }

void Player::startShooting() { shooting = true; }

void Player::updateCooldown(float deltaTime) {
  if (shootCooldown > 0.0f) {
    shootCooldown -= deltaTime;
  }
}

float Player::getShootCooldown() { return shootCooldown; }

void Player::resetCooldown() { // cuando acaba de salir una bala
  shootCooldown = equipped.cooldown;
}

bool Player::isShooting() { return shooting; }

std::tuple<float, float, float, float, float, float> Player::shoot() {
  timeLastBullet = 0.0f;
  if (typeEquipped == WeaponType::PRIMARY) {
    bulletsPrimary = bulletsPrimary - 1;
  } else if (typeEquipped == WeaponType::SECONDARY) {
    bulletsSecondary = bulletsSecondary - 1;
  }
  Hitbox hb = getHitbox();

  float origin_x = hb.x + hb.width / 2.0f;
  float origin_y = hb.y + hb.height / 2.0f;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(getRotation() - getSpreadAngle(),
                                             getRotation() + getSpreadAngle());
  float angle = dist(gen);
  float angle_rad = angle * M_PI / 180.0f;
  float max_distance;

  max_distance = equipped.maxRange;
  float target_x = origin_x + cos(angle_rad) * max_distance;
  float target_y = origin_y + sin(angle_rad) * max_distance;

  return std::make_tuple(max_distance, origin_x, origin_y, target_x, target_y,
                         angle);
}

int Player::getBulletsPerShoot() { return equipped.bulletsPerShoot; }

float Player::getSpreadAngle() { return equipped.spreadAngle; }

std::pair<float, float> Player::getDamageRange() const{
  return std::make_pair(equipped.minDamage, equipped.maxDamage);
}

void Player::changeWeapon(WeaponType newEquippedWeapon) {
  if (newEquippedWeapon == WeaponType::PRIMARY) {
    equipped = primaryWeapon;
  } else if (newEquippedWeapon == WeaponType::SECONDARY) {
    equipped = secondaryWeapon;
  } else {
    equipped = knife;
  }
}

void Player::replaceWeapon(WeaponName weapon) {
  primaryWeapon = Weapons::getWeapon(weapon);
}

uint32_t Player::getLastMoveId() const { return lastMoveId; }
void Player::setLastMoveId(uint32_t id) { lastMoveId = id; }

WeaponName Player::getPrimaryWeaponName() const { return primaryWeapon.name; }
WeaponName Player::getSecondaryWeaponName() const {
  return secondaryWeapon.name;
}

int Player::getBulletsPrimary() const { return bulletsPrimary; }

int Player::getBulletsSecondary() const { return bulletsSecondary; }

int Player::getMoney() const { return this->money; }

void Player::updateMoney(int value) { money += value; }

void Player::updatePrimaryBullets(int value) {
  bulletsPrimary += value;
}
void Player::updateSecondaryBullets(int value) {
  bulletsSecondary += value;
}

void Player::resetPrimaryBullets() { // llenar cargador
  bulletsPrimary = primaryWeapon.maxAmmo;
}
void Player::resetSecondaryBullets() { // llenar cargador
  bulletsSecondary = secondaryWeapon.maxAmmo;
}

WeaponType Player::getTypeEquipped() { return typeEquipped; }

float Player::getTimeLastBullet() { return timeLastBullet; }

void Player::updateTimeLastBullet(float deltaTime) {
  timeLastBullet += deltaTime;
}

void Player::resetTimeLastBullet() { timeLastBullet = 0; }

Weapon Player::getEquipped() { return equipped; }

int Player::getBurstFireBullets() { return burstFireBullets; }
void Player::updateBurstFireBullets(int value) { burstFireBullets += value; }
bool Player::getHasTheSpike() { return hasTheSpike; }

bool Player::isPlanting() { return planting; }

void Player::updateIsPlanting(bool isPlanting) { planting = isPlanting; }

bool Player::getAlreadyShot() { return alreadyShot; }

void Player::setAlreadyShot(bool value) { alreadyShot = value; }