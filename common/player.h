#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include "hitbox.h"
#include "structures.h"
#include "weapons.h"
#include <cmath>
#include <random>
#include <string>
class Player {
private:
  std::string name;
  float x, y;
  Hitbox hitbox;
  Role role;
  int money = 1000;
  Weapon equipped = Weapons::AK47;
  WeaponType typeEquipped = WeaponType::PRIMARY;
  Weapon knife = Weapons::Knife;
  Weapon primaryWeapon = Weapons::None;
  Weapon secondaryWeapon = Weapons::Glock;
  uint32_t bulletsPrimary = 10;
  uint32_t bulletsSecondary = 10;
  float rotation;
  float health = 100;
  float vx = 0, vy = 0;
  float aceleration = 0;
  bool hasTheSpike = false;
  bool shooting = false;
  bool planting = false;
  bool alreadyShot = false;
  float shootCooldown = 0.0f;
  float timeLastBullet = 0.0f;
  int burstFireBullets = 1;
  float lastVx = 0.0f;
  float lastVy = 0.0f;
  float slideTimer = 0.0f;
  uint32_t lastMoveId = 0;

public:
  Player(const std::string &name)
      : name(name), x(0), y(0), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},
        role(Role::COUNTER_TERRORIST), rotation(0) {}

  void changeWeapon(WeaponType newEquippedWeapon);
  bool getAlreadyShot();
  float getX() const;
  float getY() const;
  int getBulletsPerShoot();
  int getBulletsPrimary() const;
  int getBulletsSecondary() const;
  int getBurstFireBullets();
  std::pair<float, float> getDamageRange() const;
  bool getHasTheSpike();
  float getHealth() const;
  const Hitbox &getHitbox() const;
  uint32_t getLastMoveId() const;
  std::string getName() const;
  int getMoney() const;
  Weapon getEquipped();
  WeaponName getPrimaryWeaponName() const;
  WeaponName getSecondaryWeaponName() const;
  WeaponType getTypeEquipped();
  float getRotation() const;
  float getShootCooldown();
  float getSpreadAngle();
  float getTimeLastBullet();
  bool isAlive() const;
  bool isPlanting();
  bool isShooting();
  void move(float deltaTime, bool onlyX, bool onlyY);
  void replaceWeapon(WeaponName weapon);
  void resetCooldown();
  void resetPrimaryBullets();
  void resetSecondaryBullets();
  void resetTimeLastBullet();
  void setAlreadyShot(bool value);
  void setHealth(float value);
  void setLastMoveId(uint32_t id);
  void setPosition(float x, float y);
  void setRole(Role new_role);
  void setRotation(float currentRotation);
  void startShooting();
  void stopShooting();
  std::tuple<float, float, float, float, float, float> shoot();
  std::pair<float, float> tryMove(float deltaTime);
  void updateAceleration(float deltaTime);
  void updateBurstFireBullets(int value);
  void updateCooldown(float deltaTime);
  void updateHealth(float value);
  void updateIsPlanting(bool isPlanting);
  void updateMoney(int value);
  void updateMovement(float deltaTime, bool onlyX, bool onlyY);
  void updateTimeLastBullet(float deltaTime);
  void updateVelocity(float vx, float vy);
  void updatePrimaryBullets(int value);
  void updateSecondaryBullets(int value);
  int getBullets();
};
#endif
