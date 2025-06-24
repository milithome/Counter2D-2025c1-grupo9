#ifndef GAME_H
#define GAME_H
#include "gameConstants.h"
#include "gameMap.h"
#include "player.h"
#include "../../common/structures.h"
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
public:
  Game(std::vector<std::vector<CellType>> game_map, GameRules &gameRules);
  bool addPlayer(const std::string &name);
  StateGame getState();
  void update(float deltaTime);
  void shotQueueClear();
  void execute(const std::string &name, Action action);
  bool isRunning() const;
  void stop();

private:
  GameRules gameRules;
  std::vector<std::shared_ptr<Player>> players;
  Team teamA;
  Team teamB;
  int roundNumber = 1;
  int roundsUntilRoleChange;
  int roundsUntilEndGame;
  Phase phase = Phase::PURCHASE;
  std::vector<std::tuple<int, int, bool>> spawnTeamTerrorist;
  std::vector<std::tuple<int, int, bool>> spawnTeamCounter;
  bool gameStart = true;
  std::vector<DroppedWeapon> droppedWeapons;
  bool running = true;
  float time = 0.0f;
  Spike spike;
  Rounds rounds;
  RoundWinner winner;
  std::queue<Shot> shot_queue;
  GameMap map;
  float elapsedTime= 0.0f;
  bool isBombPlanted = false;
  float timeUntilPlant;
  float timeUntilDefuse;
  float timePlanting = 0.0f;
  float timeDefusing = 0.0f;
  float timeUntilBombExplode;
  float purchaseDuration;
  float timeToPlantBomb;
  float timeUntilNewRound;
  float timeUntilEndRunning;
  bool endGame= false;
  void dropWeapon(const Weapon &weapon, float x, float y);
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
  void buyBullet(const std::string &name, WeaponType type);
  void updatePlayerMovement(Player &player, float deltaTime) const;
  void updateDefusing(const std::string &name, float deltaTime);
  void updatePlanting(const std::string &name, float deltaTime);
  void shoot(const std::string &shooterName, float deltaTime);
  void applyDamageToPlayer(const Player &shooter, Player &target,
                           float distance);
  std::tuple<float, float, float, float>
  static getPlayerHitbox(const Player &player);
  static PlayerCellBounds getCellBounds(float x, float y, float width,
                                 float height);
  static bool rectsOverlap(float ax, float ay, float aw, float ah, float bx, float by,
                    float bw, float bh);
  void movePlayer(const std::string &name, float vx, float vy);
  void stopShooting(const std::string &name);
  void grab(const std::string &name);
  void placePlayerInSpawnTeam(Player &player);
  void updateGamePhase(float deltaTime);
  void updatePlayers(float deltaTime);
  void updateRounds();
  char checkRoundWinner() const;
  void handleEndRound(char winnerTeam, TypeEndRound type);
  void handlePlayerDeath(Player &target); 
  void placeTeamsInSpawn();
  static void subtractAmmo(Player &shooter, int bullets);
  Bullet simulateBullet(Player &shooter);
  void updateRotation(const std::string &name, float currentRotation);
  Entity getPlayerState(const std::string &name);
  Entity getBombState() const;
  static Entity getDroppedWeaponState(const DroppedWeapon &dw);
  int getWeaponPrice(WeaponName name);
};
#endif
