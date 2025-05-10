#ifndef PLAYER_H
#define PLAYER_H
#include "gameConstants.h"
#include <string>
class Player {
private:
  std::string name;
  int id;
  float x, y;
  Role role;

public:
  Player(const std::string &name, int id) : name(name), id(id) {}
  std::string getName() const;
  int getId() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaX, float deltaY);
};

#endif