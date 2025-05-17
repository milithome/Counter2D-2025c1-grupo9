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
        }
        case ActionType::POINT_TO: {
            const PointToAction& point = std::get<PointToAction>(action.data);
            // Convert float to bytes
            static_assert(sizeof(float) == 4, "Unexpected float size");
            uint32_t fbits;
            std::memcpy(&fbits, &point.value, sizeof(float));
            fbits = htonl(fbits);
            buffer.insert(buffer.end(),
                          reinterpret_cast<uint8_t*>(&fbits),
                          reinterpret_cast<uint8_t*>(&fbits) + sizeof(fbits));
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

void Protocol::send_initial_data() {
    uint8_t type = Type::INITIAL_DATA;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending INITIAL_DATA message");
    }

    // mapa ???
}

void Protocol::send_state(const std::vector<Entity>& entities) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::STATE);

    uint16_t size = htons(entities.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& entity : entities) {
        buffer.push_back(static_cast<uint8_t>(entity.type));

        uint16_t name_len = htons(entity.name.size());
        buffer.push_back(reinterpret_cast<uint8_t*>(&name_len)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&name_len)[1]);
        buffer.insert(buffer.end(), entity.name.begin(), entity.name.end());

        uint32_t x_net, y_net;
        std::memcpy(&x_net, &entity.x, sizeof(float));
        std::memcpy(&y_net, &entity.y, sizeof(float));
        x_net = htonl(x_net);
        y_net = htonl(y_net);

        uint8_t* x_ptr = reinterpret_cast<uint8_t*>(&x_net);
        uint8_t* y_ptr = reinterpret_cast<uint8_t*>(&y_net);
        buffer.insert(buffer.end(), x_ptr, x_ptr + sizeof(float));
        buffer.insert(buffer.end(), y_ptr, y_ptr + sizeof(float)); 
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending STATE message");
    }
}

void Protocol::send_state_lobby(const std::vector<std::string>& players) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::STATE_LOBBY);

    uint16_t size = htons(players.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& player : players) {
        uint16_t name_size = htons(player.size());
        buffer.push_back(reinterpret_cast<uint8_t*>(&name_size)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&name_size)[1]);
        buffer.insert(buffer.end(), player.begin(), player.end());
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending STATE_LOBBY message");
    }
}

void Protocol::send_response(const Response& response) {
    std::vector<uint8_t> buffer;
    buffer.push_back(response.type);

    if (response.type == Type::CREATE || response.type == Type::JOIN || response.type == Type::LEAVE) {
        buffer.push_back(response.result);
    } else if (response.type == Type::LIST) {
        uint16_t size = htons(response.partidas.size());
        buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);
        for (const auto& partida : response.partidas) {
            uint16_t size = htons(partida.size());
            buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
            buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);
            buffer.insert(buffer.end(), partida.begin(), partida.end());
        }
    } else {
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

void Protocol::recv_initial_data() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving INITIAL_DATA message");
    }

    if (type != Type::INITIAL_DATA) {
        throw std::runtime_error("Invalid INITIAL_DATA message type");
    }

    // mapa ???
}

std::vector<Entity> Protocol::recv_state() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving STATE message");
    }

    if (type != Type::STATE) {
        throw std::runtime_error("Invalid STATE message type");
    }

    uint16_t size;
    if (skt.recvall(&size, sizeof(size)) == 0) {
        throw std::runtime_error("Error receiving STATE size");
    }
    size = ntohs(size);

    std::vector<Entity> entities(size);
    for (uint16_t i = 0; i < size; ++i) {
        uint8_t entity_type;
        if (skt.recvall(&entity_type, sizeof(entity_type)) == 0) {
            throw std::runtime_error("Error receiving entity type");
        }
        entities[i].type = static_cast<EntityType>(entity_type);

        uint16_t name_len;
        if (skt.recvall(&name_len, sizeof(name_len)) == 0) {
            throw std::runtime_error("Error receiving entity name size");
        }
        name_len = ntohs(name_len);
        std::vector<uint8_t> name_buf(name_len);
        if (skt.recvall(name_buf.data(), name_len) == 0) {
            throw std::runtime_error("Error receiving entity name");
        }
        entities[i].name.assign(name_buf.begin(), name_buf.end());

        uint32_t x_net, y_net;
        if (skt.recvall(&x_net, sizeof(x_net)) == 0 || skt.recvall(&y_net, sizeof(y_net)) == 0) {
            throw std::runtime_error("Error receiving entity coordinates");
        }

        x_net = ntohl(x_net);
        y_net = ntohl(y_net);

        float x, y;
        std::memcpy(&x, &x_net, sizeof(float));
        std::memcpy(&y, &y_net, sizeof(float));

        entities[i].x = x;
        entities[i].y = y;
    }

    return entities;
}

std::vector<std::string> Protocol::recv_state_lobby() {
    std::lock_guard<std::mutex> lock(mtx);
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving STATE_LOBBY message");
    }

    if (type != Type::STATE_LOBBY) {
        throw std::runtime_error("Invalid STATE_LOBBY message type");
    }

    uint16_t size;
    if (skt.recvall(&size, sizeof(size)) == 0) {
        throw std::runtime_error("Error receiving STATE_LOBBY size");
    }
    size = ntohs(size);

    std::vector<std::string> players(size);
    for (uint16_t i = 0; i < size; ++i) {
        uint16_t name_size;
        if (skt.recvall(&name_size, sizeof(name_size)) == 0) {
            throw std::runtime_error("Error receiving player name size");
        }
        name_size = ntohs(name_size);

        std::vector<uint8_t> name(name_size);
        if (skt.recvall(name.data(), name_size) == 0) {
            throw std::runtime_error("Error receiving player name");
        }
        players[i].assign(name.begin(), name.end());
    }

    return players;
}

Response Protocol::recv_response() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving response type");
    }

    Response response;
    response.type = static_cast<Type>(type);

    if (response.type == Type::CREATE || response.type == Type::JOIN || response.type == Type::LEAVE) {
        if (skt.recvall(&response.result, sizeof(response.result)) == 0) {
            throw std::runtime_error("Error receiving response result");
        }
    } else if (response.type == Type::LIST) {
        uint16_t size_net;
        if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
            throw std::runtime_error("Error receiving response size");
        }
        uint16_t size = ntohs(size_net);
        for (uint16_t i = 0; i < size; ++i) {
            uint16_t name_size;
            if (skt.recvall(&name_size, sizeof(name_size)) == 0) {
                throw std::runtime_error("Error receiving partida name size");
            }
            name_size = ntohs(name_size);

            std::vector<uint8_t> name(name_size);
            if (skt.recvall(name.data(), name_size) == 0) {
                throw std::runtime_error("Error receiving partida name");
            }
            response.partidas.emplace_back(name.begin(), name.end());
        }
    } else {
        throw std::runtime_error("Invalid response type");
    }

    return response;
}

Message Protocol::recv_message() {
    std::lock_guard<std::mutex> lock(mtx);
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving message type");
    }

    Message message;
    message.type = static_cast<Type>(type);

    if (message.type == Type::CREATE || message.type == Type::JOIN) {
        uint16_t size;
        if (skt.recvall(&size, sizeof(size)) == 0) {
            throw std::runtime_error("Error receiving message size");
        }
        size = ntohs(size);
        message.size = size;

        std::vector<uint8_t> name(size);
        if (skt.recvall(name.data(), size) == 0) {
            throw std::runtime_error("Error receiving message name");
        }
        message.name.assign(name.begin(), name.end());
        // si no funciona usar:
        // message.name = std::string(name.begin(), name.end());
    } else if (message.type == Type::ACTION) {
        uint8_t action_type;
        if (skt.recvall(&action_type, sizeof(action_type)) == 0) {
            throw std::runtime_error("Error receiving action type");
        }

        Action action;
        action.type = static_cast<ActionType>(action_type);

        if (action.type == ActionType::MOVE) {
            int32_t x, y;
            uint32_t delta_bits;
            if (skt.recvall(&x, sizeof(x)) == 0 || 
                skt.recvall(&y, sizeof(y)) == 0 ||
                skt.recvall(&delta_bits, sizeof(delta_bits))) {
                throw std::runtime_error("Error receiving MOVE parameters");
            }
            x = ntohl(x);
            y = ntohl(y);
            delta_bits = ntohl(delta_bits);

            float deltaTime;
            std::memcpy(&deltaTime, &delta_bits, sizeof(float));

            action.data = MoveAction{x, y, deltaTime};

        } else if (action.type == ActionType::POINT_TO) {
            uint32_t fbits;
            if (skt.recvall(&fbits, sizeof(fbits)) == 0) {
                throw std::runtime_error("Error receiving POINT parameter");
            }
            fbits = ntohl(fbits);
            float value;
            std::memcpy(&value, &fbits, sizeof(float));
            action.data = PointToAction{value};

        } else {
            throw std::runtime_error("Unknown ActionType received");
        }

        message.action = std::move(action);
    } else if (message.type == Type::LIST || message.type == Type::LEAVE) {
        // No hay datos adicionales para LIST ni LEAVE 
    } else {
        throw std::runtime_error("Invalid message type");
    }

    return message;
}