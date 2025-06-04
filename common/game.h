#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "store.h"
#include "gameMap.h"
#include "structures.h"
#include "gameConstants.h"
#include "team.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>
#include <queue>
#include <random>
#include <string>
#include <vector>

class Game {
private:
  std::vector<Player> players;
  Team teamA;
  Team teamB;
  int roundNumber = 1;
  int roundsUntilRoleChange = INITIAL_ROUNDS_UNTIL_ROLE_CHANGE;
  int roundsUntilEndGame = INITIAL_ROUNDS_UNTIL_END_GAME;
  Phase phase= Phase::PURCHASE;
  std::vector<std::pair<int, int>> spawnTeamTerrorist;
  std::vector<std::pair<int, int>> spawnTeamCounter;
  bool running = true;
  float time;
  Spike spike;
  std::queue<Shot> shot_queue;
  GameMap map;
  Player &findPlayerByName(const std::string &name);
  void makeShot(Player &shooter, const std::string &shooterName);
  void plantBomb(const std::string &name);
  void stopPlantBomb(const std::string &name);
  void defuseBomb(const std::string &name);
  void stopDefuse(const std::string &name);
  void changeWeapon(const std::string &name, WeaponType type);
  std::optional<std::pair<float, float>>
  rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const;
  void buyWeapon(const std::string &name, WeaponName weaponName);
  void buyBullet(const std::string &name, WeaponType weaponName);
  bool isColliding(float x, float y, float width, float height) const;
  void updatePlayerMovement(Player &player, float deltaTime);
  void updateDefusing(const std::string &name);
  void updatePlanting(const std::string &name);
  void shoot(const std::string &shooterName, float deltaTime);
  void applyDamageToPlayer(const Player& shooter, Player& target, float distance);
  std::vector<std::pair<int, int>> findSpawnTeam(bool teamA);
  void placePlayerInSpawnTeam(Player& player, const std::vector<std::pair<int, int>>& spawn);
  float randomFloatInRange(float min, float max);
  void updateGamePhase(float deltaTime);
  float bombElapsedTime = 0.0f;
  float plantingElapsedTime = 0.0f;
  float purchaseElapsedTime = 0.0f;
  bool isBombPlanted = false;
  float timeUntilBombExplode = BOMB_DURATION;
  float purchaseDuration= PURCHASE_DURATION;
  float timeToPlantBomb= TIME_TO_PLANT;
  void updateRounds();
  int checkRoundWinner();
  void placeTeamsInSpawn();

public:
  Game(std::vector<std::vector<CellType>> game_map);
  
  // son privados
  void movePlayer(const std::string &name, float vx, float vy, uint32_t id);
  void updateRotation(const std::string &name, float currentRotation);
  void stopShooting(const std::string &name);

  // fin privados
  void
  updatePlayerPosition(const std::string &name, float x,
                       float y); // público por tema de sincronizacion cliente
  bool addPlayer(const std::string &name);
  StateGame getState();
  Entity getPlayerState(const std::string& name);
  std::vector<std::pair<WeaponName, int>> getStore();
  void update(float deltaTime);
  void execute(const std::string &name, Action action);
  bool isRunning();
  void stop();
  void updateTime(float currentTime);
  float getRotation(const std::string &name);
  float getX(const std::string &name);
  float getY(const std::string &name);

  Shot shotQueuePop();
  bool shotQueueIsEmpty();
};

#endif
