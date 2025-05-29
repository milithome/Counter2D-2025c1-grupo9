#include <gtest/gtest.h>
#include "../common/utilities/map.h"

TEST(MapTest, LoadMap) {
    std::string route = "../assets/maps/default.yaml";
    Map map(route);

    auto mapReceived = map.get_background_map();

    EXPECT_EQ(map.get_rows(), 10);
    EXPECT_EQ(map.get_cols(), 15);

    EXPECT_EQ(mapReceived[0][0], 1);
    EXPECT_EQ(mapReceived[1][1], 0);
    EXPECT_EQ(mapReceived[2][2], 1);
    EXPECT_EQ(mapReceived[4][4], 0);
}

TEST(MapTest, BackgroundLegendLookup) {
    std::string route = "../assets/maps/default.yaml";
    Map map(route);

    EXPECT_EQ(map.get_background_legend(0).id, "sand");
    EXPECT_EQ(map.get_background_legend(1).id, "water");
    EXPECT_EQ(map.get_background_legend(2).id, "spawn_team_a");
    EXPECT_EQ(map.get_background_legend(3).id, "spike_site_a");
    EXPECT_EQ(map.get_background_legend(99).id, "unknown");
}

TEST(MapTest, ManualConstruction) {
    std::vector<std::vector<uint16_t>> bg = {
        {1, 1},
        {0, 2}
    };
    std::vector<std::vector<uint16_t>> obj = {
        {0, 0},
        {0, 1}
    };

    std::unordered_map<uint16_t, MapLegendEntry> bg_legend = {
        {0, {"floor", "assets/tiles/floor.png"}}, {1, {"wall", "assets/tiles/wall.png"}}, {2, {"spawn", "assets/tiles/spawn.png"}}
    };
    std::unordered_map<uint16_t, MapLegendEntry> obj_legend = {
        {0, {"spike", "assets/objects/spike.png"}}, {1, {"crate", "assets/objects/crate.png"}}
    };

    Map map(bg, obj, bg_legend, obj_legend);

    EXPECT_EQ(map.get_rows(), 2);
    EXPECT_EQ(map.get_cols(), 2);
    EXPECT_EQ(map.get_background_map()[1][1], 2);
    EXPECT_EQ(map.get_object_map()[1][1], 1);
    EXPECT_EQ(map.get_object_legend(1).id, "crate");
}

TEST(MapTest, InvalidConstructionThrows) {
    std::vector<std::vector<uint16_t>> bg = {{1, 1}};
    std::vector<std::vector<uint16_t>> obj = {{0}};

    std::unordered_map<uint16_t, MapLegendEntry> empty;

    EXPECT_THROW({
        Map map(bg, obj, empty, empty);
    }, std::invalid_argument);
}
