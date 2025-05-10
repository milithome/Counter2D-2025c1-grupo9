#ifndef TEAM_H
#define TEAM_H

#include "gameConstants.h"
#include "player.h"
#include <vector>

class Team {
private:
  Role currentRole;
  std::vector<Player> players;
  int roundsWon = 0;

public:
  void addPlayer(const Player &player);

  const std::vector<Player> &getPlayers() const;

  int getRoundsWon() const;

  void incrementRoundsWon();

  void setRole(Role rol);

  bool invertRole();
};

#endif
