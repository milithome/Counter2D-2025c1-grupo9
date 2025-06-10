#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H
#include "structures.h"
enum class Role { TERRORIST, COUNTER_TERRORIST };

struct Position {
    float x;
    float y;
};
struct Spike {
    Position position;
    BombState state;
};

struct PlayerCellBounds {
    int leftCell;
    int rightCell;
    int topCell;
    int bottomCell;
};

struct DroppedWeapon {
  WeaponName name;
  float x;
  float y;
};

#define SPEED 5.0f
#define MAX_PLAYERS_PER_TEAM 5
#define PLAYER_WIDTH 0.7f
#define PLAYER_HEIGHT 0.7f
#define WEAPON_WIDTH 0.7f
#define WEAPON_HEIGHT 0.7f
#define BOMB_WIDTH 0.7f
#define BOMB_HEIGHT 0.7f

#define AMMO_PRICE 40
#define MAX_HEALTH 200

const float ACELERATION_RATE = 35.0f;
const float MAX_ACELERATION = 3.0f;
const float MIN_ACELERATION = -2.0f;
const float SLIDE_DURATION = 0.15f;

const float PURCHASE_DURATION = 20.0f;
const float BOMB_DURATION = 40.0f;
const float TIME_TO_PLANT = 90.0f;
const float TIME_UNTIL_PLANT = 5.0f;
const float TIME_UNTIL_DEFUSE = 5.0f;

const int INITIAL_ROUNDS_UNTIL_ROLE_CHANGE = 5;
const int INITIAL_ROUNDS_UNTIL_END_GAME = 10;

#endif
