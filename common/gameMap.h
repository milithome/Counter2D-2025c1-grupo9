#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include <utility>
#include "structures.h"

class GameMap {
private:
    std::vector<std::vector<CellType>> map;

public:
    GameMap(std::vector<std::vector<CellType>> game_map);

    std::vector<std::pair<int, int>> findSpawnTeam(bool teamA);

    bool isColliding(float x, float y, float width, float height) const;

    bool isSpikeSite(int row, int col) const;

    bool isValidCell(int row, int col) const;

    bool isBlocked(int row, int col) const;
};

#endif
