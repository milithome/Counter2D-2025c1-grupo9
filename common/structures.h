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
    CREATE,
    JOIN,
    LIST,
    ACTION,
    LEAVE,
    INITIAL_DATA,
    STATE,
    STATE_LOBBY,
    NAME,
    START,
    FINISH,
    DISCONNECT 
};

enum Phase {
    PURCHASE,
    BOMB_PLANTING,
    BOMB_DEFUSING
};

// Tipos de entidades del juego
enum EntityType { PLAYER };

// Representación de una entidad en el mundo del juego
struct Entity {
    EntityType type;
    std::string name;
    float x;
    float y;
    float rotation;
};

// Acciones posibles del jugador
enum class ActionType {
    MOVE,
    POINT_TO,
    FINISH,
    SHOOT
};

struct MoveAction {
    int x;
    int y;
    float deltaTime;
};

struct PointToAction {
    float value;
};

struct ShootAction {};

using ActionData = std::variant<std::monostate,MoveAction, PointToAction,ShootAction>;

struct Action {
    ActionType type;
    ActionData data;
};

struct StateGame {
    Phase phase;
    std::vector<Entity> entities;
};

struct InitialData {
    MapData data;
    std::vector<std::string> players;
};

struct LobbyList{
    std::vector<std::string> lobbies;
};

struct StateLobby{
    std::vector<std::string> players;
};

// Mensaje enviado por el cliente al servidor
struct Message {
  Type type;
  uint16_t size;
  std::string name;
  Action action;
};

// Respuesta enviada por el servidor al cliente
struct Response {
  Type type;
  uint8_t result;
  StateGame stateGame;
  InitialData initialData;
  LobbyList lobbyList;
  StateLobby stateLobby;
  std::string message;
};

enum class LobbyRequestType {
    LEAVE,
    JOIN,
    START
};

struct LobbyRequest {
  LobbyRequestType type;
  std::string playerName;
};

struct ActionRequest {
    Action action;
    std::string playerName;
};

struct LobbyChannels {
    std::shared_ptr<Queue<LobbyEvent>> toLobby;
    std::shared_ptr<Queue<LobbyEvent>> fromLobby;
};

struct GameChannels {
    std::shared_ptr<Queue<ActionRequest>> toGame;
    std::shared_ptr<Queue<ActionRequest>> fromGame;
};

#endif
