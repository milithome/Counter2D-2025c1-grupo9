#include "player.h"
#include "structures.h"
#include "team.h"
#include <cstdint>
#ifndef GAME_H
#define GAME_H

class Game {
private:
  std::vector<Player> players;
  Team team1;
  Team team2;
  Player &findPlayerByName(const std::string &name);
  bool running = true;
  float time;
  Phase phase;
  std::queue<Bullet> bullet_queue;
  std::vector<std::vector<CellType>> game_map;
  void makeShot(Player& shooter, const std::string& shooterName);

public:
  Game(std::vector<std::vector<CellType>> game_map) : game_map(game_map) {}
  bool addPlayer(const std::string &name);
  void updatePlayerPosition(const std::string &name, float x, float y);
  StateGame getState();
  bool isRunning();
  void stop();
  void shoot(const std::string &shooterName, float deltaTime);
  void update(float deltaTime);
  void execute(const std::string &name, Action action);
  void updateTime(float currentTime);
  void updateRotation(const std::string &name, float currentRotation);
  float getRotation(const std::string &name);
  std::vector<std::pair<WeaponName, int>> getStore();

  float getX(const std::string &name);
  float getY(const std::string &name);

  void movePlayer(const std::string &name, float vx, float vy, uint32_t id);
  void stopShooting(const std::string &name);
  void changeWeapon(const std::string &name, WeaponType type);
  Bullet bulletQueuePop();
  bool bulletQueueIsEmpty();
  void buyWeapon(const std::string &name,WeaponName weaponName);
  void buyBullet(const std::string &name,WeaponType weaponName);
};

#endif
