#include "player.h"
#include "store.h"
#include "structures.h"
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
#ifndef GAME_H
#define GAME_H

class Game {
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  bool running = true;
  float time;
  Phase phase;
  Spike spike;
  std::queue<Bullet> bullet_queue;
  std::vector<std::vector<CellType>> map;
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

public:
  Game(std::vector<std::vector<CellType>> game_map)
      : map(std::move(game_map)) {}

  // son privados
  void movePlayer(const std::string &name, float vx, float vy, uint32_t id);
  void updateRotation(const std::string &name, float currentRotation);
  void stopShooting(const std::string &name);

  // fin privados
  void
  updatePlayerPosition(const std::string &name, float x,
                       float y); // publico por tema de sincronizacion cliente
  bool addPlayer(const std::string &name);
  StateGame getState();
  std::vector<std::pair<WeaponName, int>> getStore();
  void update(float deltaTime);
  void execute(const std::string &name, Action action);
  bool isRunning();
  void stop();
  void updateTime(float currentTime);
  float getRotation(const std::string &name);
  float getX(const std::string &name);
  float getY(const std::string &name);

  Bullet bulletQueuePop();
  bool bulletQueueIsEmpty();
};

#endif
