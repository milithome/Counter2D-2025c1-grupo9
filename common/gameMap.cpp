#include "gameMap.h"
#include <cmath>

GameMap::GameMap(std::vector<std::vector<CellType>> game_map) : map(std::move(game_map)) {}

std::vector<std::pair<int, int>> GameMap::findSpawnTeam(bool teamA) {
    std::vector<std::pair<int, int>> result;

    for (size_t row = 0; row < map.size(); ++row) {
        for (size_t col = 0; col < map[row].size(); ++col) {
            if (teamA && map[row][col] == CellType::SpawnTeamA) {
                result.emplace_back(row, col);
            }
            if (!teamA && map[row][col] == CellType::SpawnTeamB) {
                result.emplace_back(row, col);
            }
        }
    }

    return result;
}

bool GameMap::isColliding(float x, float y, float width, float height) const {
    const float epsilon = 0.1f;
    float left = x + epsilon;
    float right = x + width - epsilon;
    float top = y + epsilon;
    float bottom = y + height - epsilon;

    int leftCell = static_cast<int>(std::floor(left));
    int rightCell = static_cast<int>(std::floor(right));
    int topCell = static_cast<int>(std::floor(top));
    int bottomCell = static_cast<int>(std::floor(bottom));

    for (int row = topCell; row <= bottomCell; ++row) {
        for (int col = leftCell; col <= rightCell; ++col) {
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
    return row >= 0 && col >= 0 && row < static_cast<int>(map.size()) && col < static_cast<int>(map[row].size());
}

bool GameMap::isBlocked(int row, int col) const {
    return map[row][col] == CellType::Blocked;
}
