#include "protocol.h"

Protocol::Protocol(Socket skt) : skt(std::move(skt)), mtx() {}

Protocol::~Protocol() {}

Protocol::Protocol(Protocol&& other) noexcept: skt(std::move(other.skt)) {}

Protocol& Protocol::operator=(Protocol&& other) noexcept {
    if (this != &other) {
        skt = std::move(other.skt);
    }
    return *this;
}

bool Protocol::has_data(int timeout_ms) const {
    std::lock_guard<std::mutex> lock(mtx);
    return skt.has_data(timeout_ms);
}

void Protocol::send_name(const std::string& name) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::NAME);

    uint16_t size = htons(name.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    buffer.insert(buffer.end(), name.begin(), name.end());

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending name");
    }
}

void Protocol::send_create(const std::string& name) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::CREATE);
    uint16_t size = htons(name.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    buffer.insert(buffer.end(), name.begin(), name.end());

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending CREATE message");
    }
}

void Protocol::send_join(const std::string& name) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::JOIN);
    uint16_t size = htons(name.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    buffer.insert(buffer.end(), name.begin(), name.end());

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending JOIN message");
    }
}

void Protocol::send_list() {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::LIST);

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending LIST message");
    }
}

void Protocol::send_action(const Action& action) {
    std::vector<uint8_t> buffer;

    buffer.push_back(Type::ACTION);
    buffer.push_back(static_cast<uint8_t>(action.type));

    switch (action.type) {
        case ActionType::MOVE: {
            const MoveAction& move = std::get<MoveAction>(action.data);
            int32_t x = htonl(move.x);
            int32_t y = htonl(move.y);
            uint32_t delta_bits;
            std::memcpy(&delta_bits, &move.deltaTime, sizeof(float));
            delta_bits = htonl(delta_bits);
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&x),
                        reinterpret_cast<uint8_t*>(&x) + sizeof(x));
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&y),
                        reinterpret_cast<uint8_t*>(&y) + sizeof(y));
            buffer.insert(buffer.end(),
                        reinterpret_cast<uint8_t*>(&delta_bits),
                        reinterpret_cast<uint8_t*>(&delta_bits) + sizeof(delta_bits));
            break;
        } case ActionType::POINT_TO: {
            const PointToAction& point = std::get<PointToAction>(action.data);
           
            uint32_t fbits;
            std::memcpy(&fbits, &point.value, sizeof(float));
            fbits = htonl(fbits);
            buffer.insert(buffer.end(),
                          reinterpret_cast<uint8_t*>(&fbits),
                          reinterpret_cast<uint8_t*>(&fbits) + sizeof(fbits));
            break;
        } case ActionType::SHOOT: {
            // Sin parametros de momento
            break;
        }
        default:
            throw std::runtime_error("Unsupported ActionType in send_accion");
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending ACCION message");
    }
}

void Protocol::send_leave_lobby() {
    uint8_t type = Type::LEAVE;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending LEAVE message");
    }
}

void Protocol::send_disconnect() {
    uint8_t type = Type::DISCONNECT;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending DISCONNECT message");
    }
}

std::vector<uint8_t> Protocol::serialize_simple(const Response& r) {
    return {static_cast<uint8_t>(r.type), r.result};
}

std::vector<uint8_t> Protocol::serialize_list(const Response& r) {
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};

    uint16_t size = htons(r.partidas.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& partida : r.partidas) {
        uint16_t len = htons(partida.size());
        buf.push_back(reinterpret_cast<uint8_t*>(&len)[0]);
        buf.push_back(reinterpret_cast<uint8_t*>(&len)[1]);
        buf.insert(buf.end(), partida.begin(), partida.end());
    }

    return buf;
}

std::vector<uint8_t> Protocol::serialize_state(const Response& r) {
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};

    uint16_t size = htons(r.entities.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& entity : r.entities) {
        buf.push_back(static_cast<uint8_t>(entity.type));

        uint16_t name_len = htons(entity.name.size());
        buf.push_back(reinterpret_cast<uint8_t*>(&name_len)[0]);
        buf.push_back(reinterpret_cast<uint8_t*>(&name_len)[1]);
        buf.insert(buf.end(), entity.name.begin(), entity.name.end());

        uint32_t x_net, y_net, rotation_net;
        std::memcpy(&x_net, &entity.x, sizeof(float));
        std::memcpy(&y_net, &entity.y, sizeof(float));
        std::memcpy(&rotation_net, &entity.rotation, sizeof(float));
        x_net = htonl(x_net);
        y_net = htonl(y_net);
        rotation_net = htonl(rotation_net);

        uint8_t* x_ptr = reinterpret_cast<uint8_t*>(&x_net);
        uint8_t* y_ptr = reinterpret_cast<uint8_t*>(&y_net);
        uint8_t* rot_ptr = reinterpret_cast<uint8_t*>(&rotation_net);
        
        buf.insert(buf.end(), x_ptr, x_ptr + sizeof(float));
        buf.insert(buf.end(), y_ptr, y_ptr + sizeof(float));
        buf.insert(buf.end(), rot_ptr, rot_ptr + sizeof(float));
    }

    return buf;
}

std::vector<uint8_t> Protocol::serialize_state_lobby(const Response& r) {
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};

    uint16_t size = htons(r.players.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& player : r.players) {
        uint16_t name_size = htons(player.size());
        buf.push_back(reinterpret_cast<uint8_t*>(&name_size)[0]);
        buf.push_back(reinterpret_cast<uint8_t*>(&name_size)[1]);
        buf.insert(buf.end(), player.begin(), player.end());
    }

    return buf;
}

std::vector<uint8_t> Protocol::serialize_initial_data(const Response& r) {
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};
    // TODO: Agregar más datos cuando se definan
    return buf;
}

void Protocol::send_response(const Response& r) {
    std::vector<uint8_t> buffer;

    switch (r.type) {
        case Type::CREATE:
        case Type::JOIN:
        case Type::LEAVE:
        case Type::START:
        case Type::FINISH:
            buffer = serialize_simple(r);
            break;

        case Type::LIST:
            buffer = serialize_list(r);
            break;

        case Type::STATE:
            buffer = serialize_state(r);
            break;

        case Type::STATE_LOBBY:
            buffer = serialize_state_lobby(r);
            break;

        case Type::INITIAL_DATA:
            buffer = serialize_initial_data(r);
            break;

        default:
            throw std::runtime_error("Invalid response type");
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending response");
    }
}

std::string Protocol::recv_name() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving name type");
    }

    if (type != Type::NAME) {
        throw std::runtime_error("Invalid message type, expected NAME");
    }

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving name size");
    }
    uint16_t size = ntohs(size_net);

    std::vector<char> name(size);
    if (skt.recvall(name.data(), size) == 0) {
        throw std::runtime_error("Error receiving name data");
    }

    return std::string(name.begin(), name.end());
}

Response Protocol::deserialize_simple(Type type) {
    Response r;
    r.type = type;
    if (skt.recvall(&r.result, sizeof(r.result)) == 0) {
        throw std::runtime_error("Error receiving result");
    }
    return r;
}

Response Protocol::deserialize_list() {
    Response r;
    r.type = Type::LIST;

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving list size");
    }
    uint16_t size = ntohs(size_net);

    for (uint16_t i = 0; i < size; ++i) {
        uint16_t name_size_net;
        if (skt.recvall(&name_size_net, sizeof(name_size_net)) == 0) {
            throw std::runtime_error("Error receiving name size");
        }
        uint16_t name_size = ntohs(name_size_net);

        std::vector<uint8_t> name_buf(name_size);
        if (skt.recvall(name_buf.data(), name_size) == 0) {
            throw std::runtime_error("Error receiving name");
        }
        r.partidas.emplace_back(name_buf.begin(), name_buf.end());
    }

    return r;
}

Response Protocol::deserialize_initial_data() {
    Response r;
    r.type = Type::INITIAL_DATA;
    // TODO
    return r;
}

Response Protocol::deserialize_state() {
    Response r;
    r.type = Type::STATE;

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving state size");
    }
    uint16_t size = ntohs(size_net);
    r.entities.resize(size);

    for (uint16_t i = 0; i < size; ++i) {
        uint8_t entity_type;
        if (skt.recvall(&entity_type, sizeof(entity_type)) == 0) {
            throw std::runtime_error("Error receiving entity type");
        }
        r.entities[i].type = static_cast<EntityType>(entity_type);

        uint16_t name_len;
        if (skt.recvall(&name_len, sizeof(name_len)) == 0) {
            throw std::runtime_error("Error receiving name length");
        }
        name_len = ntohs(name_len);

        std::vector<uint8_t> name_buf(name_len);
        if (skt.recvall(name_buf.data(), name_len) == 0) {
            throw std::runtime_error("Error receiving name");
        }
        r.entities[i].name.assign(name_buf.begin(), name_buf.end());

        uint32_t x_net, y_net, rotation_net;
        if (skt.recvall(&x_net, sizeof(x_net)) == 0 || 
            skt.recvall(&y_net, sizeof(y_net)) == 0 ||
            skt.recvall(&rotation_net, sizeof(rotation_net)) == 0) {
            throw std::runtime_error("Error receiving coordinates");
        }
        x_net = ntohl(x_net);
        y_net = ntohl(y_net);
        rotation_net = ntohl(rotation_net);
        std::memcpy(&r.entities[i].x, &x_net, sizeof(float));
        std::memcpy(&r.entities[i].y, &y_net, sizeof(float));
        std::memcpy(&r.entities[i].rotation, &rotation_net, sizeof(float));
    }

    return r;
}

Response Protocol::deserialize_state_lobby() {
    Response r;
    r.type = Type::STATE_LOBBY;

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving state_lobby size");
    }
    uint16_t size = ntohs(size_net);
    r.players.resize(size);

    for (uint16_t i = 0; i < size; ++i) {
        uint16_t name_size_net;
        if (skt.recvall(&name_size_net, sizeof(name_size_net)) == 0) {
            throw std::runtime_error("Error receiving player name size");
        }
        uint16_t name_size = ntohs(name_size_net);

        std::vector<uint8_t> name_buf(name_size);
        if (skt.recvall(name_buf.data(), name_size) == 0) {
            throw std::runtime_error("Error receiving player name");
        }
        r.players[i].assign(name_buf.begin(), name_buf.end());
    }

    return r;
}

Response Protocol::recv_response() {
    uint8_t type_byte;
    if (skt.recvall(&type_byte, sizeof(type_byte)) == 0) {
        throw std::runtime_error("Error receiving message type");
    }

    Type type = static_cast<Type>(type_byte);

    switch (type) {
        case Type::CREATE:
        case Type::JOIN:
        case Type::LEAVE:
        case Type::START:
        case Type::FINISH:
            return deserialize_simple(type);
        case Type::LIST:
            return deserialize_list();
        case Type::STATE:
            return deserialize_state();
        case Type::STATE_LOBBY:
            return deserialize_state_lobby();
        case Type::INITIAL_DATA:
            return deserialize_initial_data();
        default:
            throw std::runtime_error("Unknown response type");
    }
}

Message Protocol::deserialize_message_with_name(Type type) {
    Message msg;
    msg.type = type;

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving name size");
    }
    uint16_t size = ntohs(size_net);
    msg.size = size;

    std::vector<uint8_t> name(size);
    if (skt.recvall(name.data(), size) == 0) {
        throw std::runtime_error("Error receiving name");
    }

    msg.name.assign(name.begin(), name.end());
    return msg;
}

MoveAction Protocol::recv_move_action() {
    int32_t x_net, y_net;
    uint32_t delta_bits;

    if (skt.recvall(&x_net, sizeof(x_net)) == 0 ||
        skt.recvall(&y_net, sizeof(y_net)) == 0 ||
        skt.recvall(&delta_bits, sizeof(delta_bits)) == 0) {
        throw std::runtime_error("Error receiving MOVE parameters");
    }

    MoveAction move;
    move.x = ntohl(x_net);
    move.y = ntohl(y_net);
    delta_bits = ntohl(delta_bits);
    std::memcpy(&move.deltaTime, &delta_bits, sizeof(float));

    return move;
}

PointToAction Protocol::recv_point_to_action() {
    uint32_t fbits;
    if (skt.recvall(&fbits, sizeof(fbits)) == 0) {
        throw std::runtime_error("Error receiving POINT_TO parameter");
    }

    fbits = ntohl(fbits);
    float value;
    std::memcpy(&value, &fbits, sizeof(float));
    return PointToAction{value};
}

ShootAction Protocol::recv_shoot_action() {
    // Si el SHOOT no lleva parámetros:
    return ShootAction{};
}

Message Protocol::deserialize_message_action() {
    Message msg;
    msg.type = Type::ACTION;

    uint8_t action_type;
    if (skt.recvall(&action_type, sizeof(action_type)) == 0) {
        throw std::runtime_error("Error receiving action type");
    }

    Action action;
    action.type = static_cast<ActionType>(action_type);

    switch (action.type) {
        case ActionType::MOVE:
            action.data = recv_move_action();
            break;

        case ActionType::POINT_TO:
            action.data = recv_point_to_action();
            break;

        case ActionType::SHOOT:
            action.data = recv_shoot_action();
            break;

        default:
            throw std::runtime_error("Unknown ActionType received");
    }

    msg.action = std::move(action);
    return msg;
}

Message Protocol::recv_message() {
    std::lock_guard<std::mutex> lock(mtx);

    uint8_t type_byte;
    if (skt.recvall(&type_byte, sizeof(type_byte)) == 0) {
        throw std::runtime_error("Error receiving message type");
    }

    Type type = static_cast<Type>(type_byte);

    Message msg{};
    switch (type) {
        case Type::CREATE:
        case Type::JOIN:
            return deserialize_message_with_name(type);
        case Type::ACTION:
            return deserialize_message_action();
        case Type::LIST:
        case Type::LEAVE:
            msg.type = type;
            return msg;
        default:
            throw std::runtime_error("Invalid message type");
    }
}

