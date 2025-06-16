#include "team.h"

void Team::addPlayer(std::shared_ptr<Player> player) {
  players.push_back(player);
}

int Team::getPlayersAlive() const {
    int aliveCount = 0;
    for (const auto& player : players) {
        if (player->isAlive()) {
            aliveCount++;
        }
    }
    return aliveCount;
}

int Team::getRoundsWon() const { return roundsWon; }

void Team::incrementRoundsWon() { roundsWon++; }

void Team::invertRole() {
  // cambia el rol de todos los jugadores del equipo al opuesto
  // llamar al cambiar de ronda
  Role newRole;
  if (currentRole == Role::TERRORIST) {
    newRole = Role::COUNTER_TERRORIST;
  } else {
    newRole = Role::TERRORIST;
  }
  for (auto &player : players) {
    player->setRole(newRole);
  }
}

void Team::updateMoneyAfterRound(int money){
  for (auto &player : players) {
    player->updateMoney(money);
  }
}

void Team::resetSpikeCarrier(){
  for (auto &player : players){
    player->setHasSpike(false);
  }
  if (currentRole == Role::TERRORIST && !players.empty()) {
    int carrier = rand() % players.size();
    players[carrier]->setHasSpike(true);
  }
}

void Team::setRole(Role role) {
  currentRole = role;
  for (auto &player : players){
    player->setRole(role);
  }
}

Role Team::getRole(){
  return currentRole;
}

void Team::restartPlayersAlive() { 
  playersAlive = players.size(); 
  for (auto &player : players){
    player->restoreHealth();
    player->setIsAlive(true);
  }
}

int Team::getTeamSize() { return players.size(); }
