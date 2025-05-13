
#include "player.h"
#include "team.h"
#include <cstdint>
#ifndef GAME_H
#define GAME_H

enum EntityType { PLAYER };
struct Entity {
  EntityType type;
  uint id;
  float x;
  float y;
};

class Game {
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  int map_width;
  int map_height;
  Player &findPlayerById(int id);

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name, const int player_id);
  void movePlayer(const uint player_id, int x, int y);
  std::vector<Entity> getState();
};

#endif
