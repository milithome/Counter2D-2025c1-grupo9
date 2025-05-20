#include "player.h"
#include "structures.h"
#include "team.h"
#include <cstdint>
#ifndef GAME_H
#define GAME_H

class Game {
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  int map_width;
  int map_height;
  Player &findPlayerByName(const std::string &name);
  bool running = true;
  float time;

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name);
  void movePlayer(const std::string &name, int x, int y, float deltaTime);
  void updatePlayerPosition(const std::string &name, int x, int y);
  std::vector<Entity> getState();
  bool isRunning();
  void stop();
  void shoot(const std::string &shooterName);
  void update(float deltaTime);
  void execute(const std::string &name, Action action);
  void updateTime(float currentTime);
  void updateRotation(const std::string &name, float currentRotation);
  float getRotation(const std::string &name);
};

#endif
