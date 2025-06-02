#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

enum class Role { TERRORIST, COUNTER_TERRORIST };

struct Position {
    float x;
    float y;
};
struct Spike {
    Position position;
    bool isPlanted;
};

#define SPEED 5.0f
#define MAX_PLAYERS_PER_TEAM 5
#define PLAYER_WIDTH 0.7f
#define PLAYER_HEIGHT 0.7f
#define AMMO_PRICE 40
const float ACELERATION_RATE = 2.0f;
const float MAX_ACELERATION = 2.0f;
const float MIN_ACELERATION = 0.0f;



#endif
