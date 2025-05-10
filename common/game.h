#include "player.h"
#include "team.h"
#ifndef GAME_H
#define GAME_H

class Game {
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  int map_width;
  int map_height;
  Player &Game::findPlayerById(int id);

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name, const int player_id);
  void movePlayer(const int player_id, int x, int y);
};

#endif