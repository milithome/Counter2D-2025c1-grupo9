#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include <mutex>
#include "socket.h"
#include "../structures.h"



class Protocol {
public:
    Protocol(Socket skt);
    ~Protocol();
    Protocol(Protocol&& other) noexcept;
    Protocol& operator=(Protocol&& other) noexcept;

    bool has_data(int timeout_ms = 0) const;

    // Metodos para enviar mensajes al server (cliente)
    void send_name(const std::string& name);
    /*
        para estos podria hacer un { void send_message(const Message& mesagge) } para generalizar
    */
    void send_create(const std::string& name);
    void send_join(const std::string& name);
    void send_list();
    void send_action(const Action& action);
    void send_leave_lobby();
    /**/
    void send_disconnect();


    // Metodos para enviar respuestas a clientes (servidor)
    void send_response(const Response& response);

    // Metodos para recibir respuestas del server (cliente)
    std::string recv_name();
    Response recv_response();

    // Metodos para recibir mensajes de clientes (servidor)
    Message recv_message();

private:
    Socket skt;
    mutable std::mutex mtx;

    std::vector<uint8_t> serialize_simple(const Response& r);
    std::vector<uint8_t> serialize_list(const Response& r);
    std::vector<uint8_t> serialize_state(const Response& r);
    std::vector<uint8_t> serialize_state_lobby(const Response& r);
    std::vector<uint8_t> serialize_initial_data(const Response& r);

    Response deserialize_simple(Type type);
    Response deserialize_list();
    Response deserialize_initial_data();
    Response deserialize_state();
    Response deserialize_state_lobby();

    Message deserialize_message_with_name(Type type);
    MoveAction recv_move_action();
    PointToAction recv_point_to_action();
    ShootAction recv_shoot_action();
    Message deserialize_message_action();
};

#endif
