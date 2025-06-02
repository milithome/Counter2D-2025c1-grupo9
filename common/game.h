#include "player.h"
#include "structures.h"
#include "team.h"
#include "store.h"
#include <cstdint>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <queue>
#ifndef GAME_H
#define GAME_H


// TODO (de manuel):
// 1. De alguna forma tenes que registrar que el jugador esta en la tienda, asi se evita que pueda disparar
//    mientras esta en el menu de compra y asi se cuando registrar los eventos de que se clickeo
//    en algun boton de la tienda (Esto igual podria ir en GameController, se puede debatir donde quedaria mejor)
// 2. No tiene q ver con el tema grafico pero si algo que note, el jugador parece q puede seguir disparando
//    incluso si tiene 0 balas (lo q despues causa el overflow y aparece que tiene un numero telefonico de balas)
// 3. No se si tengo forma de ver que arma tengo equipada?? capaz si esta y no lo revise pero aviso por las dudas
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
  std::vector<std::vector<CellType>> map;
  void makeShot(Player& shooter, const std::string& shooterName);
  void plantBomb(const std::string &name);
  void stopPlantBomb(const std::string &name);
  void updatePlanting(const std::string &name);
  void defuseBomb(const std::string &name);
  void updateDefusing(const std::string &name);
  void stopDefuse(const std::string &name);
  std::optional<std::pair<float, float>> rayHitsWall(float x0, float y0, float x1, float y1, float maxDist) const;
  Spike spike;

public:
  Game(std::vector<std::vector<CellType>> game_map) : map(std::move(game_map)){}
  bool addPlayer(const std::string &name);
  void updatePlayerPosition(const std::string &name, float x, float y);
  StateGame getState();
  Entity getPlayerState(const std::string& name);
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
  bool isColliding(float x, float y, float width, float height) const;
  void updatePlayerMovement(Player& player, float deltaTime);
};

#endif
