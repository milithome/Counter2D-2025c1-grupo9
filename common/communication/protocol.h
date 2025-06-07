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
    void send_start();
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

    void serialize_action_move(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_point_to(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_shoot(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_stop_shooting(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_buy_bullet(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_buy_weapon(const Action& action, std::vector<uint8_t>& buffer);
    void serialize_action_change_weapon(const Action& action, std::vector<uint8_t>& buffer);

    std::vector<uint8_t> serialize_simple(const Response& r);
    std::vector<uint8_t> serialize_list(const Response& r);
    void serialize_player_data(std::vector<uint8_t>& buf, const PlayerData& pdata);
    void serialize_bomb_data(std::vector<uint8_t>& buf, const BombData& bdata);
    void serialize_weapon_data(std::vector<uint8_t>& buf, const WeaponData& wdata);
    void serialize_entity(std::vector<uint8_t>& buf, const Entity& entity);
    void serialize_bullet(std::vector<uint8_t>& buf, const Bullet& b);
    void serialize_shot(std::vector<uint8_t>& buf, const Shot& s);
    std::vector<uint8_t> serialize_state(const Response& r);
    std::vector<uint8_t> serialize_state_lobby(const Response& r);
    void serialize_2d_vector(const std::vector<std::vector<CellType>>& matrix, std::vector<uint8_t>& buf);
    void serialize_2d_vector(const std::vector<std::vector<uint16_t>>& matrix, std::vector<uint8_t>& buf);
    void serialize_legend_entry(const MapLegendEntry& entry, std::vector<uint8_t>& buf);
    void serialize_legend(const std::unordered_map<uint16_t, MapLegendEntry>& legend, std::vector<uint8_t>& buf);
    void serialize_string(const std::string& str, std::vector<uint8_t>& buf); // esta y la de abajo de podrian reutilizar en otros lugares
    void serialize_string_vector(const std::vector<std::string>& vec, std::vector<uint8_t>& buf);
    void serialize_map_data(const MapData& map, std::vector<uint8_t>& buf);
    std::vector<uint8_t> serialize_initial_data(const Response& r);

    Response deserialize_simple(Type type);
    Response deserialize_list();
    MapData deserialize_map_data();
    MapLegendEntry deserialize_legend_entry();
    std::unordered_map<uint16_t, MapLegendEntry> deserialize_legend();
    std::vector<std::vector<CellType>> deserialize_celltype_2d_vector();
    std::vector<std::vector<uint16_t>> deserialize_2d_vector();
    std::string deserialize_string();
    std::vector<std::string> deserialize_string_vector();
    Response deserialize_initial_data();
    PlayerData recv_player_data();
    BombData recv_bomb_data();
    WeaponData recv_weapon_data();
    Bullet deserialize_bullet();
    Shot deserialize_shot();
    Entity deserialize_entity_player(float x, float y);
    Entity deserialize_entity_bomb(float x, float y);
    Entity deserialize_entity_weapon(float x, float y);
    Entity deserialize_entity();
    Response deserialize_state();
    Response deserialize_state_lobby();

    Message deserialize_message_with_name(Type type);
    MoveAction recv_move_action();
    PointToAction recv_point_to_action();
    BuyBulletAction recv_buy_bullet_action();
    BuyWeaponAction recv_buy_weapon_action();
    ChangeWeaponAction recv_change_weapon_action();
    Message deserialize_message_action();
};

#endif
