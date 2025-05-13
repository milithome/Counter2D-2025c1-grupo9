#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include "hitBox.h"
#include <cmath>
#include <string>
class Player {
private:
  std::string name;
  uint id;
  float x, y;
  Hitbox hitbox;
  Role role;
  float rotation;

public:
  Player(const std::string &name, uint id)
    : name(name), id(id), x(0), y(0), rotation(0), role(Role::COUNTER_TERRORIST), hitbox{x, y, PLAYER_WIDTH, PLAYER_HEIGHT} {}

  std::string getName() const;
  uint getId() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaX, float deltaY, float deltaTime);
  void setPosition(float x, float y);
  float getRotation() const;
  void setRotation(float currentRotation);
};

#endif
