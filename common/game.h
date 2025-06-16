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
  std::vector<std::shared_ptr<Player>> players;
  Team teamA;
  Team teamB;
  int roundNumber = 1;
  int roundsUntilRoleChange = INITIAL_ROUNDS_UNTIL_ROLE_CHANGE;
  int roundsUntilEndGame = INITIAL_ROUNDS_UNTIL_END_GAME;
  Phase phase= Phase::PURCHASE;
  std::vector<std::tuple<int, int, bool>> spawnTeamTerrorist;
  std::vector<std::tuple<int, int, bool>> spawnTeamCounter;
  bool gameStart= true;
  std::vector<DroppedWeapon> droppedWeapons;
  bool running = true;
  float time;
  Spike spike;
  std::queue<Shot> shot_queue;
  GameMap map;
  void dropWeapon(const Weapon& weapon, float x, float y);

  Player &findPlayerByName(const std::string &name);
  void makeShot(Player &shooter);
  void plantBomb(const std::string &name);
  void stopPlantBomb(const std::string &name);
  void defuseBomb(const std::string &name);
  void stopDefuse(const std::string &name);
  void resetSpawn();
  void changeWeapon(const std::string &name, WeaponType type);
  std::optional<std::pair<float, float>>
  rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const;
  void buyWeapon(const std::string &name, WeaponName weaponName);
  void buyBullet(const std::string &name, WeaponType weaponName);
  void updatePlayerMovement(Player &player, float deltaTime);
  void updateDefusing(const std::string &name, float deltaTime);
  void updatePlanting(const std::string &name, float deltaTime);
  void shoot(const std::string &shooterName, float deltaTime);
  void applyDamageToPlayer(const Player& shooter, Player& target, float distance);
  std::tuple<float, float, float, float> getPlayerHitbox(const Player& player) const;
  PlayerCellBounds getCellBounds(float x,float y, float width, float height) const;
  bool rectsOverlap(float ax, float ay, float aw, float ah,
                  float bx, float by, float bw, float bh);
  void movePlayer(const std::string &name, float vx, float vy, uint32_t id);
  void stopShooting(const std::string &name);
  void grab(const std::string &name);
  void placePlayerInSpawnTeam(Player& player);
  float randomFloatInRange(float min, float max);
  void updateGamePhase(float deltaTime);
  float bombElapsedTime = 0.0f;
  float plantingElapsedTime = 0.0f;
  float purchaseElapsedTime = 0.0f;
  bool isBombPlanted = false;
  float timeUntilPlant = TIME_UNTIL_PLANT;
  float timeUntilDefuse = TIME_UNTIL_DEFUSE;
  float timePlanting = 0.0f;
  float timeDefusing = 0.0f;
  float timeUntilBombExplode = BOMB_DURATION;
  float purchaseDuration= PURCHASE_DURATION;
  float timeToPlantBomb= TIME_TO_PLANT;
  float timeUntilNewRound = TIME_UNTIL_NEW_ROUND;
  float endRoundElapsedTime =0.0f;
  void updateRounds();
  int checkRoundWinner();
  
  void placeTeamsInSpawn();

public:
  Game(std::vector<std::vector<CellType>> game_map);
  
  void
  updatePlayerPosition(const std::string &name, float x,
                       float y);
  void updatePlayerHealth(const std::string &name, int health);
  void updatePrimaryWeapon(const std::string &name, WeaponName weapon); 
  void updateRotation(const std::string &name, float currentRotation);
  bool addPlayer(const std::string &name);
  StateGame getState();
  Entity getPlayerState(const std::string& name);
  std::vector<std::pair<WeaponName, int>> getStore();
  void update(float deltaTime);
  void execute(const std::string &name, Action action);
  bool isRunning();
  void stop();
  void defuse();
  void updateTime(float currentTime);
  float getRotation(const std::string &name);
  float getX(const std::string &name);
  float getY(const std::string &name);
  void plant(float x, float y);
  void addDroppedWeapon(float x, float y, WeaponName weapon);
  Shot shotQueuePop();
  bool shotQueueIsEmpty();
  void shotQueueClear();
};

#endif
