#include "team.h"
#include <iostream>

void Team::addPlayer(Player &player) {
  players.push_back(&player);
  std::cout << "[DEBUG] Added player " << player.getName() << " to team with role " 
            << (currentRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
}

int Team::getPlayersAlive() const {
  std::cout << "[DEBUG] getPlayersAlive start" << roundsWon << std::endl;
  int aliveCount = 0;
  for (const Player* p : players) {
    if (p->isAlive()) {
      aliveCount++;
    }
  }
  std::cout << "[DEBUG] getPlayersAlive end" << roundsWon << std::endl;
  return aliveCount;
}

int Team::getRoundsWon() const { 
  std::cout << "[DEBUG] Team rounds won: " << roundsWon << std::endl;
  return roundsWon; 
}

void Team::incrementRoundsWon() {
  roundsWon++;
  std::cout << "[DEBUG] Incremented rounds won to " << roundsWon << std::endl;
}

void Team::invertRole() {
  Role newRole = (currentRole == Role::TERRORIST) ? Role::COUNTER_TERRORIST : Role::TERRORIST;
  std::cout << "[DEBUG] Inverting role from " 
            << (currentRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST")
            << " to " << (newRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
  
  currentRole = newRole;
  for (auto &player : players) {
    player->setRole(newRole);
    std::cout << "[DEBUG] Player " << player->getName() << " role set to " 
              << (newRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
  }
}

void Team::resetSpikeCarrier(){
  std::cout << "[DEBUG] Resetting spike carriers for team " 
            << (currentRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;

  for (Player* p : players){
    p->setHasSpike(false);
    std::cout << "[DEBUG] Player " << p->getName() << " spike set to false" << std::endl;
  }

  if (currentRole == Role::TERRORIST && !players.empty()) {
    int carrier = rand() % players.size();
    players[carrier]->setHasSpike(true);
    std::cout << "[DEBUG] Player " << players[carrier]->getName() << " assigned spike" << std::endl;
  }
}

void Team::setRole(Role role) {
  currentRole = role;
  std::cout << "[DEBUG] Setting team role to " << (role == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
  for (Player* p : players){
    p->setRole(role);
    std::cout << "[DEBUG] Player " << p->getName() << " role set to " 
              << (role == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
  }
}

Role Team::getRole(){
  std::cout << "[DEBUG] Getting team role: " << (currentRole == Role::TERRORIST ? "TERRORIST" : "COUNTER_TERRORIST") << std::endl;
  return currentRole;
}

void Team::restartPlayersAlive() { 
  playersAlive = players.size(); 
  std::cout << "[DEBUG] Restarting players alive count to " << playersAlive << std::endl;
  for (Player* p : players){
    p->restoreHealth();
    p->setIsAlive(true);
    std::cout << "[DEBUG] Player " << p->getName() << " health restored and set alive" << std::endl;
  }
}

int Team::getTeamSize() { 
  std::cout << "[DEBUG] Team size: " << players.size() << std::endl;
  return players.size(); 
}
