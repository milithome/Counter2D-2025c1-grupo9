#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "common/utilities/map.h"
#include "common/utilities/queue.h"
#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

// Tipos de mensajes que pueden enviarse
enum Type {
  NAME,            // mandar name
  LIST,            // mandar/recibir lista de lobbies
  CREATE,          // crear un lobby y recibir confirmacion
  JOIN,            // unirse a un lobby y recibir confirmacion
  LEAVE,           // salir de un lobby y recibir confirmacion
  STATE_LOBBY,     // recibir estado del lobby
  LOBBY_READY,     // recibir que el lobby esta en condiciones
  NOT_LOBBY_READY, // recibir que el lobby estaba en condiciones y ya no
  START,           // mandar iniciar partida
  INITIAL_DATA,    // recibir datos inicales
  ACTION,          // mandar accion
  STATE,           // recibir estado
  FINISH,          // recibir fin de la partida
  DISCONNECT       // mandar desconeccion
};

// Tipos de armas disponibles en el juego
enum class WeaponType {
  PRIMARY,
  SECONDARY,
  KNIFE,
};

enum WeaponName { AK47, M3, AWP, GLOCK, KNIFE, NONE };

enum tSkin {
  PHOENIX,
  L337_KREW,
  ARCTIC_AVENGER,
  GUERRILLA,
};

enum ctSkin { SEAL_FORCE, GERMAN_GSG9, UKSAS, FRENCH_GIGN };

// Tipos de entidades del juego
enum EntityType {
  PLAYER,
  BOMB,
  WEAPON,
};

struct Inventory {
  WeaponName primary;
  WeaponName secondary;
  uint32_t bulletsPrimary;
  uint32_t bulletsSecondary;
  bool has_the_bomb;
};

struct PlayerData {
  std::string name;
  float rotation;
  int money;
  int health;
  Inventory inventory;
  WeaponType equippedWeapon;
  bool alive;
  bool terrorist;
};

enum BombState {
  INVENTORY,
  DROPPED,
  PLANTED,
  DEFUSED,
};

struct BombData {
  BombState state;
};

struct WeaponData {
  WeaponName weapon;
};

using EntityData =
    std::variant<std::monostate, PlayerData, BombData, WeaponData>;

// Representación de una entidad en el mundo del juego
struct Entity {
  EntityType type;
  float x;
  float y;
  EntityData data;
};

// Acciones posibles del jugador
enum class ActionType {
  SELECT_T_SKIN,
  SELECT_CT_SKIN,
  SELECT_MAP,
  MOVE,
  POINT_TO,
  SHOOT,
  STOP_SHOOTING,
  PLANT,
  STOP_PLANTING,
  DEFUSE,
  STOP_DEFUSING,
  BUY_BULLET,
  BUY_WEAPON,
  GRAB,
  CHANGE_WEAPON,
  FINISH
};

struct SelectTSkin {
  tSkin terroristSkin;
};

struct SelectCTSkin {
  ctSkin counterTerroristSkin;
};

struct SelectMap {
  std::string name; // por ahora solo podra elegir mapas que el server tenga (?)
};

struct MoveAction {
  int vx;
  int vy;
};

struct PointToAction {
  float value;
};

struct BuyBulletAction {
  WeaponType type;
};

struct BuyWeaponAction {
  WeaponName weapon;
};

struct ChangeWeaponAction {
  WeaponType type;
};

using ActionData =
    std::variant<std::monostate, SelectTSkin, SelectCTSkin, SelectMap,
                 MoveAction, PointToAction, BuyBulletAction, BuyWeaponAction,
                 ChangeWeaponAction>;

struct Action {
  ActionType type;
  ActionData data;
};

enum TypeEndRound { BOMB_DEFUSED, DEAD_TEAM, BOMB_EXPLODED, BOMB_NOT_PLANTED };

// Estado del juego
enum Phase { PURCHASE, BOMB_PLANTING, BOMB_DEFUSING, END_ROUND };
struct RoundWinner {
  char team;
  TypeEndRound typeEndRound;
};

struct Rounds {
  uint16_t roundsWonTeamA;
  uint16_t roundsWonTeamB;
  uint16_t currentRound;
  RoundWinner winner;
};

enum Impact { HUMAN, BLOCK, NOTHING };

struct Bullet {
  float target_x;
  float target_y;
  float angle;
  Impact impact;
};

struct Shot {
  float origin_x;
  float origin_y;
  std::vector<Bullet> bullets;
  WeaponName weapon;
};

struct StateGame {
  Phase phase;
  std::vector<Entity> entities;
  std::queue<Shot> shots;
  Rounds rounds;
};

struct PlayerInfo {
  std::string name;
  tSkin terroristSkin;
  ctSkin counterTerroristSkin;
};

struct WeaponInfo {
  WeaponName name;
  int price;
  uint32_t maxAmmo;
};

struct Shop {
  std::vector<WeaponName> weapons;
  int primaryAmmoPrice;
  int secondaryAmmoPrice;
};

struct Times {
  float purchase_duration;
  float bomb_duration;
  float time_to_plant;
  float time_until_new_round;
};

// Datos iniciales del juego, como el mapa
struct InitialData {
  MapData data;
  std::vector<PlayerInfo> players;
  std::vector<WeaponInfo> weaponsInfo;
  Shop shop;
  Times times;
};

// Lista de lobbies disponibles
struct LobbyList {
  std::vector<std::string> lobbies;
};

// Estado del lobby, como los jugadores en el lobby
struct StateLobby {
  std::vector<std::string> players;
};

using ResponseData =
    std::variant<std::monostate, LobbyList, StateLobby, InitialData, StateGame>;

// Respuesta enviada por el servidor al cliente
struct Response {
  Type type;
  uint8_t result;
  ResponseData data;
  std::string message;
};

// Mensaje enviado por el cliente al servidor
struct Message {
  Type type;
  uint16_t size;
  std::string name;
  Action action;
  std::string clientName;
};

/* Estructuras de configuracion */
struct ServerConfig {
  std::string port;
  int tick_rate;
  uint32_t max_events_per_tick;
};

struct Weapon {
  WeaponName name;
  int price;
  bool purchasable;
  int minDamage;
  int maxDamage;
  bool burstFire;
  int bulletsPerBurst;
  float burstDelay;
  float spreadAngle;
  int bulletsPerShoot;
  float maxRange;
  uint32_t maxAmmo;
  float cooldown;
};

struct GameRules {
  int max_players_per_team;
  int min_players_per_team;
  int rounds_until_role_change;
  int rounds_until_end_game;

  int max_health;
  int max_bullets;

  float speed;

  float purchase_duration;
  float bomb_duration;
  float time_to_plant;
  float time_until_plant;
  float time_until_defuse;
  float time_until_new_round;

  int ammo_price;

  int money_winner;
  int money_loser;

  int initial_money;
  int initial_primary_ammo;
  int initial_secondary_ammo;

  std::map<WeaponName, Weapon> weapons;
};

struct WindowConfig {
  bool fullscreen;
  int width;
  int height;
};

struct FOVConfig {
  float angle_deg;
  float opacity;
};

struct ClientConfig {
  WindowConfig windowConfig;
  FOVConfig fovConfig;
};

#endif
