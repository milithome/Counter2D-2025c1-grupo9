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
  int size = 0;
  int playersAlive;

public:
  void addPlayer(const Player &player);

  int getRoundsWon() const;

  void incrementRoundsWon();

  void setRole(Role rol);

  void invertRole();

  Role getRole();

  int getTeamSize();

  void restartPlayersAlive();

  int getPlayersAlive() const;
};

#endif
