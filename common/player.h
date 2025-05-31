#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include "hitbox.h"
#include "weapons.h"
#include "structures.h"
#include <cmath>
#include <string>
#include <random>
class Player {
private:
  std::string name;
  float x, y;
  Hitbox hitbox;
  Role role;
  int money = 200;
  Weapon equipped = Weapons::Glock;
  WeaponType weaponEquipped = WeaponType::SECONDARY;
  Weapon knife= Weapons::Knife;
  Weapon primaryWeapon= Weapons::AK47;
  Weapon secondaryWeapon = Weapons::Glock;
  int bulletsPrimary;
  int bulletsSecondary;
  float rotation;
  float health;
  float vx = 0, vy = 0;
  bool shooting = false;
  float shootCooldown = 0.0f;
  uint32_t lastMoveId = 0;
  float timeLastBullet = 0.0f;
  int burstFireBullets=1;

public:
  Player(const std::string &name)
      : name(name), x(0), y(0), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},role(Role::COUNTER_TERRORIST), rotation(0) {}

  std::string getName() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaTime);
  void setPosition(float x, float y);
  float getRotation() const;
  void setRotation(float currentRotation);
  const Hitbox &getHitbox() const;
  void updateHealth(float value);
  void setHealth(float value);
  float getHealth() const;
  bool isAlive() const;
  void updateMovement(float deltaTime);
  void updateVelocity(float vx, float vy);
  void stopShooting();
  void startShooting();
  void updateCooldown(float deltaTime);
  float getShootCooldown();
  void resetCooldown();
  bool isShooting();
  std::tuple<float, float, float, float, float, float> shoot();
  int getBulletsPerShoot();
  float getSpreadAngle();
  std::pair<float, float> getDamageRange();
  void changeWeapon(WeaponType newEquippedWeapon);
  void replaceWeapon(WeaponName weapon);
  WeaponName getPrimaryWeaponName() const;
  WeaponName getSecondaryWeaponName() const;
  uint32_t getLastMoveId() const;
  void setLastMoveId(uint32_t id);
  int getBulletsPrimary() const;
  int getBulletsSecondary() const;
  int getMoney();
  void updateMoney(int value);
  void updatePrimaryBullets();
  void updateSecondaryBullets();
  WeaponType getWeaponEquipped();
  float getTimeLastBullet();
  void updateTimeLastBullet(float deltaTime);
  Weapon getEquipped();
  int getBurstFireBullets();
  void updateBurstFireBullets(int value);
  void resetTimeLastBullet();
};

#endif
