#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

struct MapLegendEntry {
    std::string id;
    std::string sprite;
};

class Map {
public:
    explicit Map(const std::string& filename);
    Map(const std::vector<std::vector<uint16_t>>& background,
        const std::vector<std::vector<uint16_t>>& objects,
        const std::unordered_map<uint16_t, MapLegendEntry>& bg_legend,
        const std::unordered_map<uint16_t, MapLegendEntry>& obj_legend);
    const std::vector<std::vector<uint16_t>>& get_background_map() const;
    const std::vector<std::vector<uint16_t>>& get_object_map() const;
    size_t get_rows() const;
    size_t get_cols() const;
    const MapLegendEntry& get_background_legend(uint16_t number) const;
    const MapLegendEntry& get_object_legend(uint16_t number) const;
private:
    std::vector<std::vector<uint16_t>> background_map;
    std::vector<std::vector<uint16_t>> object_map;
    std::unordered_map<uint16_t, MapLegendEntry> legend_background;
    std::unordered_map<uint16_t, MapLegendEntry> legend_objects;

    void load_map(const std::string& filename);
};

#endif // MAP_H
