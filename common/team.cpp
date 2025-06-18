#include "team.h"

Team::Team(GameRules& gameRules) : gameRules(gameRules) {}

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
  Role newRole;
  if (currentRole == Role::TERRORIST) {
    newRole = Role::COUNTER_TERRORIST;
  } else {
    newRole = Role::TERRORIST;
  }
  for (auto &player : players) {
    player->setRole(newRole);
    player->replaceWeapon(WeaponName::NONE);
    player->changeWeapon(WeaponType::SECONDARY);
    player->setPrimaryBullets(gameRules.initial_primary_ammo); 
    player->setSecondaryBullets(gameRules.initial_secondary_ammo);
    player->setMoney(gameRules.initial_money); 
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
