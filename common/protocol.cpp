#include "protocol.h"

Protocol::Protocol(Socket skt) : skt(std::move(skt)) {}

Protocol::~Protocol() {}

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
    uint8_t type = Type::LIST;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending LIST message");
    }
}

void Protocol::send_accion(Action action) {
    std::vector<uint8_t> buffer;

    buffer.push_back(Type::ACCION);
    buffer.push_back(static_cast<uint8_t>(action));

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending ACCION message");
    }
}

void Protocol::send_leave() {
    uint8_t type = Type::LEAVE;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending LEAVE message");
    }
}

void Protocol::send_initial_data() {
    uint8_t type = Type::INITIAL_DATA;

    // mapa ???

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending INITIAL_DATA message");
    }
}

void Protocol::send_state(const std::vector<Entity>& entities) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::STATE);

    uint16_t size = htons(entities.size());
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buffer.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& entity : entities) {
        buffer.push_back(static_cast<uint8_t>(entity.type));
        buffer.push_back(entity.id);
        uint16_t x = htons(static_cast<uint16_t>(entity.x));
        buffer.push_back(reinterpret_cast<uint8_t*>(&x)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&x)[1]);
        uint16_t y = htons(static_cast<uint16_t>(entity.y));
        buffer.push_back(reinterpret_cast<uint8_t*>(&y)[0]);
        buffer.push_back(reinterpret_cast<uint8_t*>(&y)[1]);
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending STATE message");
    }
}

void Protocol::send_response(const Response& response) {
    std::vector<uint8_t> buffer;
    buffer.push_back(response.type);

    if (response.type == Type::CREATE || response.type == Type::JOIN) {
        buffer.push_back(response.result);
    } else if (response.type == Type::LIST) {
        uint16_t size = htons(response.size);
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

std::vector<Entity>& Protocol::recv_state() {
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

        uint id;
        if (skt.recvall(&id, sizeof(id)) == 0) {
            throw std::runtime_error("Error receiving entity ID");
        }
        entities[i].id = id;

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

Response Protocol::recv_response() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving response type");
    }

    Response response;
    response.type = static_cast<Type>(type);

    if (response.type == Type::CREATE || response.type == Type::JOIN) {
        if (skt.recvall(&response.result, sizeof(response.result)) == 0) {
            throw std::runtime_error("Error receiving response result");
        }
    } else if (response.type == Type::LIST) {
        uint16_t size;
        if (skt.recvall(&size, sizeof(size)) == 0) {
            throw std::runtime_error("Error receiving response size");
        }
        size = ntohs(size);
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
    } else if (message.type == Type::ACCION) {
        if (skt.recvall(&message.action, sizeof(message.action)) == 0) {
            throw std::runtime_error("Error receiving action");
        }
    } else {
        throw std::runtime_error("Invalid message type");
    }

    return message;
}