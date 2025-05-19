#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include "hitbox.h"
#include <cmath>
#include <string>
class Player {
private:
  std::string name;
  float x, y;
  Hitbox hitbox;
  Role role;
  float rotation;
  float health;

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
};

#endif
