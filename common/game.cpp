#include "game.h"
#include <cmath>
#include <iostream>

bool Game::addPlayer(const std::string &name, const int id) {
  if (team1.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team1.addPlayer(Player(name, id));
    return true;
  }
  if (team2.getTeamSize() < MAX_PLAYERS_PER_TEAM) {
    team2.addPlayer(Player(name, id));
    return true;
  }
  return false;
}

Player &Game::findPlayerById(int id) {
  for (auto &player : players) {
    if (player.getId() == id)
      return player;
  }
  throw std::runtime_error("Player not found");
}

void Game::movePlayer(int player_id, int x, int y) {
  findPlayerById(player_id).move(x * MOVEMENT, y * MOVEMENT);
}
