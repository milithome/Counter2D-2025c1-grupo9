#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <cstdint>

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

// Acciones posibles del jugador
enum Action {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN
};

struct ActionEvent
{
    Action type;
    std::string playerName;
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
