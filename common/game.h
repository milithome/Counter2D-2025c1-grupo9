#include "player.h"
#include "structures.h"
#include "team.h"
#include <cstdint>
#ifndef GAME_H
#define GAME_H

struct ShotEvent {
  float origin_x;
  float origin_y;
  float target_x;
  float target_y;
  float angle; //grados
};

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
  void movePlayer(const std::string &name, float vx, float vy);
  void stopShooting(const std::string &name);
  std::queue<ShotEvent> shot_event_queue;

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


  ShotEvent shotEventQueuePop();
  bool shotEventQueueIsEmpty();
};

#endif
