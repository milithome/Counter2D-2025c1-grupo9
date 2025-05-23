#include "map.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>

Map::Map(const std::string& filename) {
    load_map(filename);
}

Map::Map(const std::vector<std::vector<uint16_t>>& background,
         const std::vector<std::vector<uint16_t>>& objects,
         const std::unordered_map<uint16_t, MapLegendEntry>& bg_legend,
         const std::unordered_map<uint16_t, MapLegendEntry>& obj_legend)
    : background_map(background),
      object_map(objects),
      legend_background(bg_legend),
      legend_objects(obj_legend) {
    
    if (background_map.empty() || object_map.empty()) {
        throw std::invalid_argument("Map matrices cannot be empty.");
    }
    if (background_map.size() != object_map.size() || background_map[0].size() != object_map[0].size()) {
        throw std::invalid_argument("Map dimensions mismatch between background and objects.");
    }
}

void Map::load_map(const std::string& filename) {
    try {
        YAML::Node root = YAML::LoadFile(filename);
        background_map.clear();
        object_map.clear();
        legend_background.clear();
        legend_objects.clear();

        const YAML::Node& legend_bg = root["legend_background"];
        for (auto it = legend_bg.begin(); it != legend_bg.end(); ++it) {
            uint16_t number = it->first.as<uint16_t>();
            const YAML::Node& entry = it->second;
            MapLegendEntry le;
            le.id = entry["id"].as<std::string>();
            le.sprite = entry["sprite"] ? entry["sprite"].as<std::string>() : "";
            legend_background[number] = le;
        }

        const YAML::Node& legend_obj = root["legend_objects"];
        for (auto it = legend_obj.begin(); it != legend_obj.end(); ++it) {
            uint16_t number = it->first.as<uint16_t>();
            const YAML::Node& entry = it->second;
            MapLegendEntry le;
            le.id = entry["id"].as<std::string>();
            le.sprite = (entry["sprite"] && !entry["sprite"].IsNull()) ? entry["sprite"].as<std::string>() : "";
            legend_objects[number] = le;
        }

        const YAML::Node& map = root["map"];
        size_t width = map["width"].as<size_t>();
        size_t height = map["height"].as<size_t>();

        const YAML::Node& background = map["background"];
        const YAML::Node& objects = map["objects"];

        if (background.size() != height || objects.size() != height) {
            throw std::runtime_error("Background or object layer height mismatch");
        }

        for (size_t i = 0; i < height; ++i) {
            const YAML::Node& row_bg = background[i];
            const YAML::Node& row_obj = objects[i];

            if (row_bg.size() != width || row_obj.size() != width) {
                throw std::runtime_error("Background or object layer width mismatch");
            }

            std::vector<uint16_t> row_background, row_objects;
            for (size_t j = 0; j < width; ++j) {
                row_background.push_back(row_bg[j].as<uint16_t>());
                row_objects.push_back(row_obj[j].as<uint16_t>());
            }
            background_map.push_back(row_background);
            object_map.push_back(row_objects);
        }
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Error loading map: " + std::string(e.what()));
    }
}

const std::vector<std::vector<uint16_t>>& Map::get_background_map() const {
    return background_map;
}

const std::vector<std::vector<uint16_t>>& Map::get_object_map() const {
    return object_map;
}

size_t Map::get_rows() const {
    return background_map.size();
}

size_t Map::get_cols() const {
    return background_map.empty() ? 0 : background_map[0].size();
}

const MapLegendEntry& Map::get_background_legend(uint16_t number) const {
    static MapLegendEntry unknown = {"unknown", ""};
    auto it = legend_background.find(number);
    return it != legend_background.end() ? it->second : unknown;
}

const MapLegendEntry& Map::get_object_legend(uint16_t number) const {
    static MapLegendEntry unknown = {"unknown", ""};
    auto it = legend_objects.find(number);
    return it != legend_objects.end() ? it->second : unknown;
}


/*
void Protocol::send_map(const Map& map) {
    const auto& mapa = map.get_map();
    uint16_t rows = htons(map.get_rows());
    uint16_t cols = htons(map.get_cols());

    // Enviar número de filas
    skt.sendall(&rows, sizeof(rows));

    // Enviar número de columnas
    skt.sendall(&cols, sizeof(cols));

    // Enviar los valores de la matriz (mapa) como uint16_t
    for (const auto& row : mapa) {
        for (uint16_t cell : row) {
            uint16_t cell_network = htons(cell); // Convertir a red endian
            skt.sendall(&cell_network, sizeof(cell_network));
        }
    }
}


std::vector<std::vector<uint16_t>> Protocol::recv_map() {
    uint16_t rows, cols;
    
    // Recibir número de filas
    if (skt.recvall(&rows, sizeof(rows)) == 0) {
        throw std::runtime_error("Error receiving rows count");
    }
    rows = ntohs(rows);

    // Recibir número de columnas
    if (skt.recvall(&cols, sizeof(cols)) == 0) {
        throw std::runtime_error("Error receiving columns count");
    }
    cols = ntohs(cols);

    // Crear la matriz
    std::vector<std::vector<uint16_t>> map(rows, std::vector<uint16_t>(cols));

    // Recibir los valores de la matriz
    for (uint16_t r = 0; r < rows; ++r) {
        for (uint16_t c = 0; c < cols; ++c) {
            uint16_t cell_network;
            if (skt.recvall(&cell_network, sizeof(cell_network)) == 0) {
                throw std::runtime_error("Error receiving map cell");
            }
            map[r][c] = ntohs(cell_network); // Convertir de red endian a host endian
        }
    }

    return map;
}
*/