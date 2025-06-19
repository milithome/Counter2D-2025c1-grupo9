#include "gameMap.h"

GameMap::GameMap(std::vector<std::vector<CellType>> game_map)
    : map(std::move(game_map)) {}

std::vector<std::tuple<int, int, bool>> GameMap::findSpawnTeam(bool teamA) {
  std::vector<std::tuple<int, int, bool>> result;

  for (size_t row = 0; row < map.size(); ++row) {
    for (size_t col = 0; col < map[row].size(); ++col) {
      if (teamA && map[row][col] == CellType::SpawnTeamA) {
        result.emplace_back(row, col, false);
      }
      if (!teamA && map[row][col] == CellType::SpawnTeamB) {
        result.emplace_back(row, col, false);
      }
    }
  }

  return result;
}

bool GameMap::isColliding(PlayerCellBounds bounds) const {

  for (int row = bounds.topCell; row <= bounds.bottomCell; ++row) {
    for (int col = bounds.leftCell; col <= bounds.rightCell; ++col) {
      if (row < 0 || col < 0 || row >= static_cast<int>(map.size()) ||
          col >= static_cast<int>(map[row].size())) {
        continue;
      }
      if (map[row][col] == CellType::Blocked) {
        return true;
      }
    }
  }
  return false;
}

bool GameMap::isSpikeSite(int row, int col) const {
  return map[row][col] == CellType::SpikeSite;
}

bool GameMap::isValidCell(int row, int col) const {
  return row >= 0 && col >= 0 && row < static_cast<int>(map.size()) &&
         col < static_cast<int>(map[row].size());
}

bool GameMap::isBlocked(int row, int col) const {
  return map[row][col] == CellType::Blocked;
}
