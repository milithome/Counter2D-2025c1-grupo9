#include "team.h"

void Team::addPlayer(const Player &player) {
  players.push_back(player);
  size++;
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
    player.setRole(newRole);
  }
}

void Team::setRole(Role rol) {
  // cuando ya hayan entrado todos los jugadores, queremos un rol inicial random
  // para cada equipo gestionar cual desde el juego
  currentRole = rol;
}

int Team::getTeamSize() { return size; }
