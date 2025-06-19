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

public:
  std::string name;
  float x, y;
  Hitbox hitbox;
  Role role;
  int money = INITIAL_MONEY;
  Weapon equipped = Weapons::Glock;
  WeaponType typeEquipped = WeaponType::SECONDARY;
  Weapon knife = Weapons::Knife;
  Weapon primaryWeapon = Weapons::None;
  Weapon secondaryWeapon = Weapons::Glock;
  uint32_t bulletsPrimary = INITIAL_PRIMARY_AMMO;
  uint32_t bulletsSecondary = INITIAL_SECONDARY_AMMO;
  float rotation;
  int health = MAX_HEALTH;
  float vx = 0, vy = 0;
  float aceleration = 0;
  bool hasTheSpike = true;
  bool shooting = false;
  bool planting = false;
  bool defusing = false;
  bool alreadyShot = false;
  float shootCooldown = 0.0f;
  float timeLastBullet = 0.0f;
  int burstFireBullets = 1;
  float lastVx = 0.0f;
  float lastVy = 0.0f;
  float slideTimer = 0.0f;
  bool alive= true;
  Player(const std::string &name)
      : name(name), x(0), y(0), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},
        role(Role::COUNTER_TERRORIST), rotation(0) {}

  void changeWeapon(WeaponType newEquippedWeapon);
  void move(float deltaTime, bool onlyX, bool onlyY);
  void replaceWeapon(WeaponName weapon);
  void resetCooldown();
  void resetPrimaryBullets();
  void resetSecondaryBullets();
  void resetTimeLastBullet();
  void restoreHealth();
  std::tuple<float, float, float, float, float, float> shoot();
  std::pair<float, float> tryMove(float deltaTime);
  void updateAceleration(float deltaTime);
  void updateBurstFireBullets(int value);
  void updateCooldown(float deltaTime);
  void updateHealth(int value);
  void updateMoney(int value);
  void updateMovement(float deltaTime, bool onlyX, bool onlyY);
  void updateTimeLastBullet(float deltaTime);
  void updateVelocity(float vx, float vy);
  void updatePrimaryBullets(int value);
  void updateSecondaryBullets(int value);
  int getBullets();
  int getBulletsPerShoot();
  std::pair<float, float> getDamageRange() const;
  float getSpreadAngle();
  void setPosition(float x, float y);
};
#endif
