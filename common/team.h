#ifndef TEAM_H
#define TEAM_H

#include "gameConstants.h"
#include "player.h"
#include <vector>

class Team {
private:
  Role currentRole;
  std::vector<std::shared_ptr<Player>> players;
  int playersAlive;

public:
  void addPlayer(std::shared_ptr<Player> player);
  void setRole(Role rol);
  void invertRole();
  Role getRole();
  int getTeamSize();
  void restartPlayersAlive();
  int getPlayersAlive() const;
  void resetSpikeCarrier();
  void updateMoneyAfterRound(int money);
};

#endif
