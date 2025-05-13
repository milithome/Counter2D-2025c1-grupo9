#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
<<<<<<< Updated upstream
    Player() {
    }
=======
  Player(const std::string &name, uint id) : name(name), id(id) {}
  std::string getName() const;
  uint getId() const;
  void setRole(Role new_role);
  float getX() const;
  float getY() const;
  void move(float deltaX, float deltaY, float deltaTime);
  void setPosition(float x, float y);
>>>>>>> Stashed changes
};

#endif 