#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include "hitbox.h"
#include "weapons.h"
#include <cmath>
#include <string>
#include <random>
class Player {
private:
  std::string name;
  float x, y;
  Hitbox hitbox;
  Role role;
  Weapon equipped = Weapons::M3;
  EquippedWeapon weaponEquipped = EquippedWeapon::SECONDARY;
  Weapon knife= Weapons::Knife;
  Weapon primaryWeapon= Weapons::M3;
  Weapon secondaryWeapon = Weapons::Glock;
  float rotation;
  float health;
  float vx = 0, vy = 0;
  bool shooting = false;
  float shootCooldown = 0.0f;
public:
  Player(const std::string &name)
      : name(name), x(0), y(0), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT},role(Role::COUNTER_TERRORIST), rotation(0) {}

  std::string getName() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaX, float deltaY, float deltaTime);
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
  void changeWeapon(EquippedWeapon newEquippedWeapon);


  uint32_t getLastMoveId() const;
};

#endif
