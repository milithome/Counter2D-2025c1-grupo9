#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <cstdint>
#include <variant>
#include "../server/queue.h"

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

using ActionData = std::variant<std::monostate,MoveAction, PointToAction>;

struct Action {
    ActionType type;
    ActionData data;
};

// Eventos para las acciones
struct ActionEvent{
    Action action;
    std::string playerName;
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
  uint16_t size;
  std::vector<Entity> entities;
  std::vector<std::string> partidas;
  uint8_t result;
  std::string message;
};

enum class LobbyEventType {
    LEAVE,
    JOIN,
    START
};

struct LobbyEvent {
  LobbyEventType type;
  std::string playerName;
};

struct LobbyChannels {
    std::shared_ptr<Queue<LobbyEvent>> toLobby;
    std::shared_ptr<Queue<LobbyEvent>> fromLobby;
};

struct GameChannels {
    std::shared_ptr<Queue<ActionEvent>> toGame;
    std::shared_ptr<Queue<ActionEvent>> fromGame;
};

#endif
