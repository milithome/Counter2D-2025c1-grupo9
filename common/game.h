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
  int map_width;
  int map_height;
  Player &findPlayerByName(const std::string &name);
  bool running = true;
  float time;
  Phase phase;
  std::queue<Bullet> bullet_queue;

public:
  Game(int width, int height) : map_width(width), map_height(height) {}
  bool addPlayer(const std::string &name);
  void updatePlayerPosition(const std::string &name, float x, float y);
  std::vector<Entity> getState();
  bool isRunning();
  void stop();
  void shoot(const std::string &shooterName);
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
  void bulletQueuePush(Bullet bullet);
  bool bulletQueueIsEmpty();
  void buyWeapon(const std::string &name,WeaponName weaponName);
  void buyBullet(const std::string &name,WeaponType weaponName);
};

#endif
