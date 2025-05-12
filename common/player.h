#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include <cmath>
#include <string>
class Player {
private:
  std::string name;
  uint id;
  float x, y;
  Role role;

public:
  Player(const std::string &name, uint id) : name(name), id(id) {}
  std::string getName() const;
  uint getId() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaX, float deltaY);
  void setPosition(float x, float y);
};

#endif