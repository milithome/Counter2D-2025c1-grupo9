#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <unordered_map>

struct MapLegendEntry {
    int x = 0;
    int y = 0;
};

enum class CellType {
    Walkable,
    Blocked,
    SpikeSite,
    SpawnTeamA,
    SpawnTeamB
};

struct MapData {
    std::string background_path;
    std::string sprite_path;

    std::vector<std::vector<CellType>> game_map;

    std::vector<std::vector<uint16_t>> tiles_map;
    std::unordered_map<uint16_t, MapLegendEntry> legend_tiles;
};

/* 
La idea de esta clase es que la instancia de Game en el Gameloop llame a : Map(const std::string& filename); y la instancia de Game del cliente explicit Map(const MapData& data);. El servidor enviara como data inicial un MapData para que el Game del cliente pueda iniciar.
*/

class Map {
public:
    Map() = default;
    explicit Map(const std::string& filename); // Este lo usara el server
    explicit Map(const MapData& data); // Este lo usara el cliente

    void load_map(const std::string& filename);

    const std::string& get_background_path() const;
    const std::string& get_sprite_path() const;

    const std::vector<std::vector<CellType>>& get_game_map() const;
    const std::vector<std::vector<uint16_t>>& get_tiles_map() const;

    size_t get_rows() const;
    size_t get_cols() const;

    const MapLegendEntry& get_tiles_legend(uint16_t number) const;

    const MapData& getMapData() const;

private:
    MapData data_;
};

#endif // MAP_H
