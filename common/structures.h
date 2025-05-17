#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <cstdint>
#include <variant>

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
    NAME    
};

// Tipos de entidades del juego
enum EntityType {
    PLAYER
};

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
    POINT_TO
};

struct MoveAction {
    int x;
    int y;
    float deltaTime;
};

struct PointToAction {
    float value;
};

using ActionData = std::variant<MoveAction, PointToAction>;

struct Action {
    ActionType type;
    ActionData data;
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
};

struct LobbyEvent {
    LobbyEventType type;
    std::string playerName;
};

#endif
