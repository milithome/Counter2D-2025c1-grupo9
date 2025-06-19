#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "../structures.h" 

GameRules load_game_rules(const std::string& path);
ServerConfig load_server_config(const std::string& path);
ClientConfig load_client_config(const std::string& path);
WeaponName stringToWeaponName(const std::string& str);

#endif
