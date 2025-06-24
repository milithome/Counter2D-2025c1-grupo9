#ifndef TEAM_H
#define TEAM_H

#include "gameConstants.h"
#include "player.h"
#include <vector>

class Team {
private:
  GameRules &gameRules;
  Role currentRole;
  std::vector<std::shared_ptr<Player>> players;
  int playersAlive;

public:
  explicit Team(GameRules &gameRules);

  void addPlayer(std::shared_ptr<Player> player);
  void setRole(Role role);
  void invertRole();
  Role getRole() const;
  int getTeamSize() const;
  void restartPlayersAlive();
  int getPlayersAlive() const;
  void resetSpikeCarrier();
  void updateMoneyAfterRound(int money);
};

#endif
