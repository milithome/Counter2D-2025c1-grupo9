#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "gameConstants.h"
#include "structures.h"
#include <cmath>
#include <utility>
#include <vector>

class GameMap {
private:
  std::vector<std::vector<CellType>> map;

public:
  GameMap(std::vector<std::vector<CellType>> game_map);

  std::vector<std::tuple<int, int, bool>> findSpawnTeam(bool teamA);

  bool isColliding(PlayerCellBounds bounds) const;

  bool isSpikeSite(int row, int col) const;

  bool isValidCell(int row, int col) const;

  bool isBlocked(int row, int col) const;
};

#endif
