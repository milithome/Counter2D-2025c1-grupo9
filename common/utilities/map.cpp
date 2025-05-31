#include "map.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <iostream>

Map::Map(const std::string& filename) {
    load_map(filename);
}

Map::Map(const MapData& data) : data_(data) {
    if (data_.game_map.empty() || data_.tiles_map.empty()) {
        throw std::invalid_argument("Game map and tiles map cannot be empty.");
    }
    if (data_.game_map.size() != data_.tiles_map.size() ||
        data_.game_map[0].size() != data_.tiles_map[0].size()) {
        throw std::invalid_argument("Map dimensions mismatch between game map and tiles map.");
    }
}

void Map::load_map(const std::string& filename) {
    try {
        YAML::Node root = YAML::LoadFile(filename);

        const YAML::Node& map = root["map"];
        data_.background_path = map["background_path"].as<std::string>();
        data_.sprite_path = map["sprite_path"].as<std::string>();

        // Cargar game map
        const YAML::Node& game = map["game"];
        size_t height = game.size();
        if (height == 0) throw std::runtime_error("Game map is empty.");
        size_t width = game[0].size();

        data_.game_map.clear();
        for (size_t i = 0; i < height; ++i) {
            const YAML::Node& row = game[i];
            if (row.size() != width) {
                throw std::runtime_error("Inconsistent game map row width.");
            }
            std::vector<uint16_t> row_vec;
            for (size_t j = 0; j < width; ++j) {
                row_vec.push_back(row[j].as<uint16_t>());
            }
            data_.game_map.push_back(std::move(row_vec));
        }

        // Cargar tiles map
        const YAML::Node& tiles = map["tiles"];
        if (tiles.size() != height) {
            throw std::runtime_error("Tiles map height mismatch.");
        }
        data_.tiles_map.clear();
        for (size_t i = 0; i < height; ++i) {
            const YAML::Node& row = tiles[i];
            if (row.size() != width) {
                throw std::runtime_error("Inconsistent tiles map row width.");
            }
            std::vector<uint16_t> row_vec;
            for (size_t j = 0; j < width; ++j) {
                row_vec.push_back(row[j].as<uint16_t>());
            }
            data_.tiles_map.push_back(std::move(row_vec));
        }

        // Cargar leyenda game
        const YAML::Node& legendGame = root["legend_game"];
        data_.legend_game.clear();
        for (auto it = legendGame.begin(); it != legendGame.end(); ++it) {
            uint16_t number = it->first.as<uint16_t>();
            const YAML::Node& entry = it->second;
            MapLegendEntry le;
            le.x = entry["x"] ? entry["x"].as<int>() : 0;
            le.y = entry["y"] ? entry["y"].as<int>() : 0;
            data_.legend_game[number] = le;
        }

        // Cargar leyenda tiles
        const YAML::Node& legendTiles = root["legend_tiles"];
        data_.legend_tiles.clear();
        for (auto it = legendTiles.begin(); it != legendTiles.end(); ++it) {
            uint16_t number = it->first.as<uint16_t>();
            const YAML::Node& entry = it->second;
            MapLegendEntry le;
            le.x = entry["x"] ? entry["x"].as<int>() : 0;
            le.y = entry["y"] ? entry["y"].as<int>() : 0;
            data_.legend_tiles[number] = le;
        }

    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Error loading map: " + std::string(e.what()));
    }
}

const std::string& Map::get_background_path() const {
    return data_.background_path;
}

const std::string& Map::get_sprite_path() const {
    return data_.sprite_path;
}

const std::vector<std::vector<uint16_t>>& Map::get_game_map() const {
    return data_.game_map;
}

const std::vector<std::vector<uint16_t>>& Map::get_tiles_map() const {
    return data_.tiles_map;
}

size_t Map::get_rows() const {
    return data_.game_map.size();
}

size_t Map::get_cols() const {
    return data_.game_map.empty() ? 0 : data_.game_map[0].size();
}

const MapLegendEntry& Map::get_game_legend(uint16_t number) const {
    static MapLegendEntry unknown = {0, 0};
    auto it = data_.legend_game.find(number);
    return it != data_.legend_game.end() ? it->second : unknown;
}

const MapLegendEntry& Map::get_tiles_legend(uint16_t number) const {
    static MapLegendEntry unknown = {0, 0};
    auto it = data_.legend_tiles.find(number);
    return it != data_.legend_tiles.end() ? it->second : unknown;
}
