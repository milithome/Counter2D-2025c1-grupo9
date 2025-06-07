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
  bool team= false; //true es A, false es B
  int money = 1000;
  Weapon equipped = Weapons::Glock;
  WeaponType typeEquipped = WeaponType::SECONDARY;
  Weapon knife = Weapons::Knife;
  Weapon primaryWeapon = Weapons::AWP;
  Weapon secondaryWeapon = Weapons::Glock;
  uint32_t bulletsPrimary = 0;
  uint32_t bulletsSecondary = 0;
  float rotation;
  int health = 100;
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
  bool alive= true;

public:
  Player(const std::string &name)
      : name(name), x(0), y(0), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},
        role(Role::COUNTER_TERRORIST), rotation(0) {}

  void changeWeapon(WeaponType newEquippedWeapon);
  bool getAlreadyShot();
  float getX() const;
  float getY() const;
  void setTeam(bool teamA);
  int getBulletsPerShoot();
  int getBulletsPrimary() const;
  int getBulletsSecondary() const;
  int getBurstFireBullets();
  std::pair<float, float> getDamageRange() const;
  bool getHasTheSpike();
  int getHealth() const;
  const Hitbox &getHitbox() const;
  uint32_t getLastMoveId() const;
  std::string getName() const;
  int getMoney() const;
  Weapon getEquipped();
  WeaponName getPrimaryWeaponName() const;
  WeaponName getSecondaryWeaponName() const;
  WeaponType getTypeEquipped() const;
  float getRotation() const;
  float getShootCooldown();
  float getSpreadAngle();
  float getTimeLastBullet();
  bool isAlive() const;
  void setIsAlive(bool isAlive);
  bool isPlanting();
  bool isShooting();
  void move(float deltaTime, bool onlyX, bool onlyY);
  void replaceWeapon(WeaponName weapon);
  void resetCooldown();
  void resetPrimaryBullets();
  void resetSecondaryBullets();
  void resetTimeLastBullet();
  void setAlreadyShot(bool value);
  void restoreHealth();
  void setLastMoveId(uint32_t id);
  void setPosition(float x, float y);
  void setRotation(float currentRotation);
  void startShooting();
  void stopShooting();
  std::tuple<float, float, float, float, float, float> shoot();
  std::pair<float, float> tryMove(float deltaTime);
  void updateAceleration(float deltaTime);
  void updateBurstFireBullets(int value);
  void updateCooldown(float deltaTime);
  void updateHealth(int value);
  void updateIsPlanting(bool isPlanting);
  void updateMoney(int value);
  void updateMovement(float deltaTime, bool onlyX, bool onlyY);
  void updateTimeLastBullet(float deltaTime);
  void updateVelocity(float vx, float vy);
  void updatePrimaryBullets(int value);
  void updateSecondaryBullets(int value);
  int getBullets();
  Role getRole();
  void setRole(Role newRole);
};
#endif
