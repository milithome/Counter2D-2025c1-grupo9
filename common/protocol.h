#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include "socket.h"
#include "structures.h"

class Protocol {
public:
    Protocol(Socket skt);
    ~Protocol();

    // Metodos para enviar mensajes (cliente)
    void send_create(const std::string& name);
    void send_join(const std::string& name);
    void send_list();
    void send_accion(Action action);
    void send_leave();

    // Metodos para enviar mensajes (servidor)
    // Podria incluir send_initial_data y send_state en send_response pero me salio hacerlo asi pq no son respuestas a peticiones q hizo el cliente
    // y no tienen sentido como respuesta a una peticion
    void send_initial_data();
    void send_state(const std::vector<Entity>& entities);
    void send_response(const Response& response);

    // Metodos para recibir mensajes (cliente)
    void recv_initial_data();
    std::vector<Entity>& recv_state();
    Response recv_response();

    // Metodos para recibir mensajes (servidor)
    Message recv_message();

private:
    Socket skt;
};

#endif
