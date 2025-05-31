#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <cstdint>
#include <variant>
#include "common/utilities/queue.h"
#include "common/utilities/map.h"

// Tipos de mensajes que pueden enviarse
enum Type {
    NAME, // mandar name
    LIST, // mandar/recibir lista de lobbies
    CREATE, // crear un lobby y recibir confirmacion
    JOIN, //unirse a un lobby y recibir confirmacion
    LEAVE, // salir de un lobby y recibir confirmacion
    STATE_LOBBY, // recibir estado del lobby
    LOBBY_READY, // recibir que el lobby se lleno
    START, // mandar iniciar partida
    INITIAL_DATA, // recibir datos inicales
    ACTION, // mandar accion
    STATE, // recibir estado
    FINISH, // recibir fin de la partida
    DISCONNECT // mandar desconeccion
};

// Tipos de armas disponibles en el juego
enum class WeaponType {
    PRIMARY,
    SECONDARY,
    KNIFE,
};

enum WeaponName {
    AK47,
    M3,
    AWP,
    GLOCK,
    KNIFE
};

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
};

struct PlayerData {
    std::string name;
    float rotation;
    uint32_t lastMoveId;
    int money;
    float health;
    Inventory inventory;
};

struct BombData {
    bool planted;
};

struct WeaponData {
    WeaponType type;
    WeaponName weapon;
};

using EntityData = std::variant<std::monostate, PlayerData, BombData, WeaponData>;

// Representación de una entidad en el mundo del juego
struct Entity {
    EntityType type;
    float x;
    float y;
    EntityData data;
};

// Acciones posibles del jugador
enum class ActionType {
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

struct MoveAction {
    uint32_t id;
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

/*
Accion sin parametros de momento:
struct ShootAction {};

struct StopShootingAction {};

struct PlantAction {};

struct StopPlantingAction {};

struct DefuseAction {};

struct StopDefusingAction {};

struct GrabAction {};
*/

using ActionData = std::variant<std::monostate, MoveAction, PointToAction, BuyBulletAction, BuyWeaponAction, ChangeWeaponAction>;

struct Action {
    ActionType type;
    ActionData data;
};

// Estado del juego
enum Phase {
    PURCHASE,
    BOMB_PLANTING,
    BOMB_DEFUSING
};

struct Bullet {
    float origin_x;
    float origin_y;
    float target_x;
    float target_y;
    float angle;
};

struct StateGame {
    Phase phase;
    std::vector<Entity> entities;
    std::queue<Bullet> bullets;
};

// Datos iniciales del juego, como el mapa
struct InitialData {
    MapData data;
    std::vector<std::string> players;
};

// Lista de lobbies disponibles
struct LobbyList{
    std::vector<std::string> lobbies;
};

// Estado del lobby, como los jugadores en el lobby
struct StateLobby{
    std::vector<std::string> players;
};

using ResponseData = std::variant<std::monostate, LobbyList, StateLobby, InitialData, StateGame>;

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
};

#endif
