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

    // Metodos para enviar mensajes (cliente)
    void send_name(const std::string& name);
    void send_create(const std::string& name);
    void send_join(const std::string& name);
    void send_list();
    void send_action(const Action& action);
    void send_leave_lobby();
    void send_disconnect();

    // void send_message(const Message& mesagge); // -> proximamente

    // Metodos para enviar mensajes (servidor)
    // Podria incluir send_initial_data y send_state en send_response pero me salio hacerlo asi pq no son respuestas a peticiones q hizo el cliente
    // y no tienen sentido como respuesta a una peticion
    void send_initial_data();
    void send_state(const std::vector<Entity>& entities);
    void send_state_lobby(const std::vector<std::string>& players);
    void send_response(const Response& response);

    // Metodos para recibir mensajes (cliente)
    std::string recv_name();
    void recv_initial_data();
    std::vector<Entity> recv_state();
    std::vector<std::string> recv_state_lobby();
    Response recv_response();

    // Metodos para recibir mensajes (servidor)
    Message recv_message();

private:
    Socket skt;
    mutable std::mutex mtx;
};

#endif
