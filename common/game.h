#include "player.h"
#include "structures.h"
#include "team.h"
#include "bullet.h"
#include <cstdint>
#ifndef GAME_H
#define GAME_H

class Game {
private:
  std::vector<Player> players;
  std::vector<Bullet> activeBullets;
  Team team1;
  Team team2;
  int map_width;
  int map_height;
  Player &findPlayerById(uint id);
  bool running = true;
  float time;

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name, const int player_id);
  void movePlayer(const uint player_id, int x, int y, float deltaTime);
  std::vector<Entity> getState();
  bool isRunning();
  void stop();
  void updateTime(float currentTime);
  void updateRotation(uint player_id, float currentRotation);
  float getRotation(uint player_id);
  void update(float deltaTime);
  void shoot(uint player_id);
};

#endif
