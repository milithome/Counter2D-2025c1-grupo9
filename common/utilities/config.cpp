#include "config.h"

ServerConfig load_server_config(const std::string& filename) {
    YAML::Node root = YAML::LoadFile(filename);
    YAML::Node server = root["server"];
    
    ServerConfig config;
    config.port = root["server"]["port"].as<std::string>();
    config.tick_rate = server["tick_rate"].as<int>();
    config.max_events_per_tick = server["max_events_per_tick"].as<uint32_t>();
    return config;
}

WeaponName stringToWeaponName(const std::string& s) {
    if (s == "AK47") return WeaponName::AK47;
    if (s == "AWP") return WeaponName::AWP;
    if (s == "M3") return WeaponName::M3;
    if (s == "GLOCK") return WeaponName::GLOCK;
    if (s == "KNIFE") return WeaponName::KNIFE;
    return WeaponName::NONE;
}

GameRules load_game_rules(const std::string& filename) {
    YAML::Node root = YAML::LoadFile(filename);
    YAML::Node game = root["game"];

    GameRules rules;

    rules.max_players_per_team = game["max_players_per_team"].as<int>();
    rules.min_players_per_team = game["min_players_per_team"].as<int>();
    rules.rounds_until_role_change = game["rounds_until_role_change"].as<int>();
    rules.rounds_until_end_game = game["rounds_until_end_game"].as<int>();

    rules.max_health = game["max_health"].as<int>();
    rules.max_bullets = game["max_bullets"].as<int>();
    rules.speed = game["speed"].as<float>();

    rules.purchase_duration = game["purchase_duration"].as<float>();
    rules.bomb_duration = game["bomb_duration"].as<float>();
    rules.time_to_plant = game["time_to_plant"].as<float>();
    rules.time_until_plant = game["time_until_plant"].as<float>();
    rules.time_until_defuse = game["time_until_defuse"].as<float>();
    rules.time_until_new_round = game["time_until_new_round"].as<float>();

    rules.ammo_price = game["ammo_price"].as<int>();
    rules.initial_money = game["initial_money"].as<int>();
    rules.money_winner = game["money_winner"].as<int>();
    rules.money_loser = game["money_loser"].as<int>();
    rules.initial_primary_ammo = game["initial_primary_ammo"].as<int>();
    rules.initial_secondary_ammo = game["initial_secondary_ammo"].as<int>();


    YAML::Node weapons_node = game["weapons"];
    for (auto it = weapons_node.begin(); it != weapons_node.end(); ++it) {
        std::string name_str = it->first.as<std::string>();
        WeaponName name = stringToWeaponName(name_str);

        YAML::Node w = it->second;

        Weapon weapon {
            name,
            w["price"].as<int>(),
            w["min_damage"].as<int>(),
            w["max_damage"].as<int>(),
            w["burst_fire"].as<bool>(),
            w["bullets_per_burst"].as<int>(),
            w["burst_delay"].as<float>(),
            w["spread_angle"].as<float>(),
            w["bullets_per_shoot"].as<int>(),
            w["max_range"].as<float>(),
            w["max_ammo"].as<uint32_t>(),
            w["cooldown"].as<float>()
        };

        rules.weapons[name] = weapon;
    }

    return rules;
}

ClientConfig load_client_config(const std::string& filename) {
    YAML::Node root = YAML::LoadFile(filename);

    WindowConfig window;
    window.fullscreen = root["client"]["fullscreen"].as<bool>();
    window.width = root["client"]["resolution"]["width"].as<int>();
    window.height = root["client"]["resolution"]["height"].as<int>();

    FOVConfig fov;
    fov.angle_deg = root["fov"]["angle_deg"].as<float>();
    fov.opacity = root["fov"]["opacity"].as<float>();

    ClientConfig client{window, fov};
    return client;
}
