#include "protocol.h"
#include <variant>

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

void Protocol::serialize_action_move(const Action& action, std::vector<uint8_t>& buffer) {
    const MoveAction& move = std::get<MoveAction>(action.data);

    uint32_t id = htonl(move.id);
    int32_t x = htonl(move.vx);
    int32_t y = htonl(move.vy);

    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&id),
                  reinterpret_cast<uint8_t*>(&id) + sizeof(id));

    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&x),
                  reinterpret_cast<uint8_t*>(&x) + sizeof(x));

    buffer.insert(buffer.end(),
                  reinterpret_cast<uint8_t*>(&y),
                  reinterpret_cast<uint8_t*>(&y) + sizeof(y));
}

void Protocol::serialize_action_point_to(const Action& action, std::vector<uint8_t>& buffer) {
    const PointToAction& point = std::get<PointToAction>(action.data);
    uint32_t fbits;
    std::memcpy(&fbits, &point.value, sizeof(float));
    fbits = htonl(fbits);

    buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&fbits), reinterpret_cast<uint8_t*>(&fbits) + sizeof(fbits));
}

void Protocol::serialize_action_buy_bullet(const Action& action, std::vector<uint8_t>& buffer) {
    const BuyBulletAction& buy = std::get<BuyBulletAction>(action.data);
    buffer.push_back(static_cast<uint8_t>(buy.type));
}

void Protocol::serialize_action_buy_weapon(const Action& action, std::vector<uint8_t>& buffer) {
    const BuyWeaponAction& buy = std::get<BuyWeaponAction>(action.data);
    buffer.push_back(static_cast<uint8_t>(buy.weapon));
}

void Protocol::serialize_action_change_weapon(const Action& action, std::vector<uint8_t>& buffer) {
    const ChangeWeaponAction& change = std::get<ChangeWeaponAction>(action.data);
    buffer.push_back(static_cast<uint8_t>(change.type));
}

void Protocol::send_action(const Action& action) {
    std::vector<uint8_t> buffer;

    buffer.push_back(Type::ACTION);
    buffer.push_back(static_cast<uint8_t>(action.type));

    switch (action.type) {
        case ActionType::MOVE:
            serialize_action_move(action, buffer);
            break;

        case ActionType::POINT_TO:
            serialize_action_point_to(action, buffer);
            break;

        case ActionType::BUY_BULLET:
            serialize_action_buy_bullet(action, buffer);
            break;

        case ActionType::BUY_WEAPON:
            serialize_action_buy_weapon(action, buffer);
            break;

        case ActionType::CHANGE_WEAPON:
            serialize_action_change_weapon(action, buffer);
            break;

        case ActionType::SHOOT:
        case ActionType::STOP_SHOOTING:
        case ActionType::PLANT:
        case ActionType::STOP_PLANTING:
        case ActionType::DEFUSE:
        case ActionType::STOP_DEFUSING:
        case ActionType::GRAB:
            break;

        default:
            throw std::runtime_error("Unsupported ActionType in send_action");
    }

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending ACTION message");
    }
}

void Protocol::send_leave_lobby() {
    uint8_t type = Type::LEAVE;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending LEAVE message");
    }
}

void Protocol::send_start() {
    uint8_t type = Type::START;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending START message");
    }
}

void Protocol::send_disconnect() {
    uint8_t type = Type::DISCONNECT;

    if (skt.sendall(&type, sizeof(type)) <= 0) {
        throw std::runtime_error("Error sending DISCONNECT message");
    }
}

std::vector<uint8_t> Protocol::serialize_simple(const Response& r) {
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};
    
    buf.push_back(static_cast<uint8_t>(r.result));

    serialize_string(r.message, buf);
    return buf;
}

std::vector<uint8_t> Protocol::serialize_list(const Response& r) {
    const LobbyList& list = std::get<LobbyList>(r.data);
    
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};

    buf.push_back(static_cast<uint8_t>(r.result));

    uint16_t size = htons(list.lobbies.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& partida : list.lobbies) {
        uint16_t len = htons(partida.size());
        buf.push_back(reinterpret_cast<uint8_t*>(&len)[0]);
        buf.push_back(reinterpret_cast<uint8_t*>(&len)[1]);
        buf.insert(buf.end(), partida.begin(), partida.end());
    }

    serialize_string(r.message, buf);

    return buf;
}

void Protocol::serialize_player_data(std::vector<uint8_t>& buf, const PlayerData& pdata) {
    uint16_t len = htons(pdata.name.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&len)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&len)[1]);
    buf.insert(buf.end(), pdata.name.begin(), pdata.name.end());

    uint32_t rot_net;
    std::memcpy(&rot_net, &pdata.rotation, sizeof(float));
    rot_net = htonl(rot_net);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&rot_net), reinterpret_cast<uint8_t*>(&rot_net) + sizeof(rot_net));

    uint32_t move_net = htonl(pdata.lastMoveId);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&move_net), reinterpret_cast<uint8_t*>(&move_net) + sizeof(move_net));

    int32_t money_net = htonl(pdata.money);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&money_net), reinterpret_cast<uint8_t*>(&money_net) + sizeof(money_net));

    int32_t health_net = ntohl(pdata.health);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&health_net), reinterpret_cast<uint8_t*>(&health_net) + sizeof(health_net));

    buf.push_back(static_cast<uint8_t>(pdata.inventory.primary));
    buf.push_back(static_cast<uint8_t>(pdata.inventory.secondary));

    uint32_t bp_net = htonl(pdata.inventory.bulletsPrimary);
    uint32_t bs_net = htonl(pdata.inventory.bulletsSecondary);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&bp_net), reinterpret_cast<uint8_t*>(&bp_net) + sizeof(bp_net));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&bs_net), reinterpret_cast<uint8_t*>(&bs_net) + sizeof(bs_net));
    buf.push_back(static_cast<uint8_t>(pdata.inventory.has_the_bomb));

    buf.push_back(static_cast<uint8_t>(pdata.equippedWeapon));
}

void Protocol::serialize_bomb_data(std::vector<uint8_t>& buf, const BombData& bdata) {
    buf.push_back(static_cast<uint8_t>(bdata.planted));
}

void Protocol::serialize_weapon_data(std::vector<uint8_t>& buf, const WeaponData& wdata) {
    buf.push_back(static_cast<uint8_t>(wdata.type));
    buf.push_back(static_cast<uint8_t>(wdata.weapon));
}

void Protocol::serialize_entity(std::vector<uint8_t>& buf, const Entity& entity) {
    buf.push_back(static_cast<uint8_t>(entity.type));

    uint32_t x_net, y_net;
    std::memcpy(&x_net, &entity.x, sizeof(float));
    std::memcpy(&y_net, &entity.y, sizeof(float));
    x_net = htonl(x_net);
    y_net = htonl(y_net);
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&x_net), reinterpret_cast<uint8_t*>(&x_net) + sizeof(x_net));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&y_net), reinterpret_cast<uint8_t*>(&y_net) + sizeof(y_net));

    switch (entity.type) {
        case PLAYER:
            serialize_player_data(buf, std::get<PlayerData>(entity.data));
            break;

        case BOMB:
            serialize_bomb_data(buf, std::get<BombData>(entity.data));
            break;

        case WEAPON:
            serialize_weapon_data(buf, std::get<WeaponData>(entity.data));
            break;

        default:
            throw std::runtime_error("Unknown EntityType in serialization");
    }
}

void Protocol::serialize_bullet(std::vector<uint8_t>& buf, const Bullet& b) {
    float values[3] = {b.target_x, b.target_y, b.angle};
    for (int i = 0; i < 3; ++i) {
        uint32_t net;
        std::memcpy(&net, &values[i], sizeof(float));
        net = htonl(net);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&net), reinterpret_cast<uint8_t*>(&net) + sizeof(net));
    }

    buf.push_back(static_cast<uint8_t>(b.impact));
}

void Protocol::serialize_shot(std::vector<uint8_t>& buf, const Shot& s) {
    float origins[2] = {s.origin_x, s.origin_y};
    for (int i = 0; i < 2; ++i) {
        uint32_t net;
        std::memcpy(&net, &origins[i], sizeof(float));
        net = htonl(net);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&net), reinterpret_cast<uint8_t*>(&net) + sizeof(net));
    }

    uint16_t bullet_count = htons(s.bullets.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&bullet_count)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&bullet_count)[1]);

    // Serialize each bullet
    for (const Bullet& b : s.bullets) {
        serialize_bullet(buf, b);
    }

    buf.push_back(static_cast<uint8_t>(s.weapon));
}

std::vector<uint8_t> Protocol::serialize_state(const Response& r) {
    const StateGame& game = std::get<StateGame>(r.data);
    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};
    buf.push_back(static_cast<uint8_t>(r.result));
    buf.push_back(static_cast<uint8_t>(game.phase));

    uint16_t size = htons(game.entities.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const Entity& entity : game.entities) {
        serialize_entity(buf, entity);
    }

    std::queue<Shot> copy = game.shots;
    uint16_t shot_count = htons(copy.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&shot_count)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&shot_count)[1]);

    while (!copy.empty()) {
        serialize_shot(buf, copy.front());
        copy.pop();
    }

    serialize_string(r.message, buf);
    return buf;
}

std::vector<uint8_t> Protocol::serialize_state_lobby(const Response& r) {
    const StateLobby& stateLobby = std::get<StateLobby>(r.data);

    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};
    buf.push_back(static_cast<uint8_t>(r.result));

    uint16_t size = htons(stateLobby.players.size());
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[0]);
    buf.push_back(reinterpret_cast<uint8_t*>(&size)[1]);

    for (const auto& player : stateLobby.players) {
        uint16_t name_size = htons(player.size());
        buf.push_back(reinterpret_cast<uint8_t*>(&name_size)[0]);
        buf.push_back(reinterpret_cast<uint8_t*>(&name_size)[1]);
        buf.insert(buf.end(), player.begin(), player.end());
    }

    serialize_string(r.message, buf);
    return buf;
}

void Protocol::serialize_string(const std::string& str, std::vector<uint8_t>& buf) {
    uint16_t len = htons(static_cast<uint16_t>(str.size()));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + sizeof(len));
    buf.insert(buf.end(), str.begin(), str.end());
}

void Protocol::serialize_string_vector(const std::vector<std::string>& vec, std::vector<uint8_t>& buf) {
    uint16_t count = htons(static_cast<uint16_t>(vec.size()));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));
    for (const auto& str : vec) {
        serialize_string(str, buf);
    }
}

void Protocol::serialize_2d_vector(const std::vector<std::vector<CellType>>& matrix, std::vector<uint8_t>& buf) {
    uint16_t rows = htons(static_cast<uint16_t>(matrix.size()));
    uint16_t cols = htons(matrix.empty() ? 0 : static_cast<uint16_t>(matrix[0].size()));

    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&rows), reinterpret_cast<uint8_t*>(&rows) + sizeof(rows));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&cols), reinterpret_cast<uint8_t*>(&cols) + sizeof(cols));

    for (const auto& row : matrix) {
        for (CellType val : row) {
            uint16_t val_net = htons(static_cast<uint16_t>(val));
            buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&val_net), reinterpret_cast<uint8_t*>(&val_net) + sizeof(val_net));
        }
    }
}

void Protocol::serialize_2d_vector(const std::vector<std::vector<uint16_t>>& matrix, std::vector<uint8_t>& buf) {
    uint16_t rows = htons(static_cast<uint16_t>(matrix.size()));
    uint16_t cols = htons(matrix.empty() ? 0 : static_cast<uint16_t>(matrix[0].size()));

    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&rows), reinterpret_cast<uint8_t*>(&rows) + sizeof(rows));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&cols), reinterpret_cast<uint8_t*>(&cols) + sizeof(cols));

    for (const auto& row : matrix) {
        for (uint16_t val : row) {
            uint16_t val_net = htons(val);
            buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&val_net), reinterpret_cast<uint8_t*>(&val_net) + sizeof(val_net));
        }
    }
}

void Protocol::serialize_legend_entry(const MapLegendEntry& entry, std::vector<uint8_t>& buf) {
    int32_t x_net = htonl(entry.x);
    int32_t y_net = htonl(entry.y);

    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&x_net), reinterpret_cast<uint8_t*>(&x_net) + sizeof(x_net));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&y_net), reinterpret_cast<uint8_t*>(&y_net) + sizeof(y_net));
}

void Protocol::serialize_legend(const std::unordered_map<uint16_t, MapLegendEntry>& legend, std::vector<uint8_t>& buf) {
    uint16_t count = htons(static_cast<uint16_t>(legend.size()));
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));

    for (const auto& [key, entry] : legend) {
        uint16_t key_net = htons(key);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&key_net), reinterpret_cast<uint8_t*>(&key_net) + sizeof(key_net));
        serialize_legend_entry(entry, buf);
    }
}

void Protocol::serialize_map_data(const MapData& map, std::vector<uint8_t>& buf) {
    serialize_string(map.background_path, buf);
    serialize_string(map.sprite_path, buf);

    serialize_2d_vector(map.game_map, buf);

    serialize_2d_vector(map.tiles_map, buf);
    serialize_legend(map.legend_tiles, buf);
}

std::vector<uint8_t> Protocol::serialize_initial_data(const Response& r) {
    const InitialData& init = std::get<InitialData>(r.data);

    std::vector<uint8_t> buf = {static_cast<uint8_t>(r.type)};
    buf.push_back(static_cast<uint8_t>(r.result));

    serialize_map_data(init.data, buf);
    serialize_string_vector(init.players, buf);

    serialize_string(r.message, buf);

    return buf;
}

void Protocol::send_response(const Response& r) {
    std::vector<uint8_t> buffer;

    switch (r.type) {
        case Type::CREATE:
        case Type::JOIN:
        case Type::LEAVE:
        case Type::START:
        case Type::LOBBY_READY:
        case Type::FINISH:
        case Type::DISCONNECT:
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
    r.data = {};
    r.message = deserialize_string();
    return r;
}

Response Protocol::deserialize_list() {
    Response r;
    r.type = Type::LIST;
    uint8_t result;
    if (skt.recvall(&result, sizeof(result)) == 0) {
        throw std::runtime_error("Error receiving list result");
    }
    r.result = result;

    LobbyList list;
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
        list.lobbies.emplace_back(name_buf.begin(), name_buf.end());
    }
    r.data = std::move(list);
    r.message = deserialize_string();
    return r;
}

std::string Protocol::deserialize_string() {
    uint16_t len_net;
    if (skt.recvall(&len_net, sizeof(len_net)) == 0)
        throw std::runtime_error("Error receiving string length");

    uint16_t len = ntohs(len_net);
    std::vector<char> buffer(len);
    if (skt.recvall(buffer.data(), len) == 0)
        throw std::runtime_error("Error receiving string data");

    return std::string(buffer.begin(), buffer.end());
}

std::vector<std::string> Protocol::deserialize_string_vector() {
    uint16_t count_net;
    if (skt.recvall(&count_net, sizeof(count_net)) == 0)
        throw std::runtime_error("Error receiving string vector count");

    uint16_t count = ntohs(count_net);
    std::vector<std::string> result;

    for (uint16_t i = 0; i < count; ++i) {
        result.push_back(deserialize_string());
    }

    return result;
}

std::vector<std::vector<CellType>> Protocol::deserialize_celltype_2d_vector() {
    uint16_t rows_net, cols_net;
    if (skt.recvall(&rows_net, sizeof(rows_net)) == 0 || skt.recvall(&cols_net, sizeof(cols_net)) == 0)
        throw std::runtime_error("Error receiving 2D vector dimensions");

    uint16_t rows = ntohs(rows_net);
    uint16_t cols = ntohs(cols_net);

    std::vector<std::vector<CellType>> matrix(rows, std::vector<CellType>(cols));
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t val_net;
            if (skt.recvall(&val_net, sizeof(val_net)) == 0)
                throw std::runtime_error("Error receiving matrix value");
            matrix[i][j] = static_cast<CellType>(ntohs(val_net));
        }
    }
    return matrix;
}

std::vector<std::vector<uint16_t>> Protocol::deserialize_2d_vector() {
    uint16_t rows_net, cols_net;
    if (skt.recvall(&rows_net, sizeof(rows_net)) == 0 || skt.recvall(&cols_net, sizeof(cols_net)) == 0)
        throw std::runtime_error("Error receiving 2D vector dimensions");

    uint16_t rows = ntohs(rows_net);
    uint16_t cols = ntohs(cols_net);

    std::vector<std::vector<uint16_t>> matrix(rows, std::vector<uint16_t>(cols));
    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            uint16_t val_net;
            if (skt.recvall(&val_net, sizeof(val_net)) == 0)
                throw std::runtime_error("Error receiving matrix value");
            matrix[i][j] = ntohs(val_net);
        }
    }
    return matrix;
}

std::unordered_map<uint16_t, MapLegendEntry> Protocol::deserialize_legend() {
    uint16_t count_net;
    if (skt.recvall(&count_net, sizeof(count_net)) == 0)
        throw std::runtime_error("Error receiving legend count");

    uint16_t count = ntohs(count_net);
    std::unordered_map<uint16_t, MapLegendEntry> legend;

    for (uint16_t i = 0; i < count; ++i) {
        uint16_t key_net;
        if (skt.recvall(&key_net, sizeof(key_net)) == 0)
            throw std::runtime_error("Error receiving legend key");
        uint16_t key = ntohs(key_net);

        legend[key] = deserialize_legend_entry();
    }

    return legend;
}

MapLegendEntry Protocol::deserialize_legend_entry() {
    int32_t x_net, y_net;
    if (skt.recvall(&x_net, sizeof(x_net)) == 0 || skt.recvall(&y_net, sizeof(y_net)) == 0)
        throw std::runtime_error("Error receiving legend entry");

    MapLegendEntry entry;
    entry.x = ntohl(x_net);
    entry.y = ntohl(y_net);
    return entry;
}

MapData Protocol::deserialize_map_data() {
    MapData map;

    map.background_path = deserialize_string();
    map.sprite_path = deserialize_string();

    map.game_map = deserialize_celltype_2d_vector();

    map.tiles_map = deserialize_2d_vector();
    map.legend_tiles = deserialize_legend();

    return map;
}

Response Protocol::deserialize_initial_data() {
    Response r;
    r.type = Type::INITIAL_DATA;
    uint8_t result;
    if (skt.recvall(&result, sizeof(result)) == 0)
        throw std::runtime_error("Error receiving initial data result");
    r.result = result;

    InitialData init;
    init.data = deserialize_map_data();
    init.players = deserialize_string_vector();
    r.data = std::move(init);

    r.message = deserialize_string();
    return r;
}

PlayerData Protocol::recv_player_data() {                
    PlayerData p;

    uint16_t name_len_net;
    if (skt.recvall(&name_len_net, sizeof(name_len_net)) == 0)
        throw std::runtime_error("Error receiving player name length");
    uint16_t name_len = ntohs(name_len_net);

    std::vector<uint8_t> name_buf(name_len);
    if (skt.recvall(name_buf.data(), name_len) == 0)
        throw std::runtime_error("Error receiving player name");
    p.name.assign(name_buf.begin(), name_buf.end());

    uint32_t rot_net;
    if (skt.recvall(&rot_net, sizeof(rot_net)) == 0)
        throw std::runtime_error("Error receiving player rotation");
    rot_net = ntohl(rot_net);
    std::memcpy(&p.rotation, &rot_net, sizeof(float));

    uint32_t id_net;
    if (skt.recvall(&id_net, sizeof(id_net)) == 0)
        throw std::runtime_error("Error receiving lastMoveId");
    p.lastMoveId = ntohl(id_net);

    int32_t money_net;
    if (skt.recvall(&money_net, sizeof(money_net)) == 0)
        throw std::runtime_error("Error receiving player money");
    p.money = ntohl(money_net);

    int32_t health_net;
    if (skt.recvall(&health_net, sizeof(health_net)) == 0)
        throw std::runtime_error("Error receiving player health");
    p.health = ntohl(health_net);

    uint8_t prim, sec, has_bomb;
    uint32_t bprim_net, bsec_net;
    if (skt.recvall(&prim, sizeof(prim)) == 0 ||
        skt.recvall(&sec, sizeof(sec)) == 0 ||
        skt.recvall(&bprim_net, sizeof(bprim_net)) == 0 ||
        skt.recvall(&bsec_net, sizeof(bsec_net)) == 0 ||
        skt.recvall(&has_bomb, sizeof(has_bomb)) == 0)
        throw std::runtime_error("Error receiving inventory");
    p.inventory.primary = static_cast<WeaponName>(prim);
    p.inventory.secondary = static_cast<WeaponName>(sec);
    p.inventory.bulletsPrimary = ntohl(bprim_net);
    p.inventory.bulletsSecondary = ntohl(bsec_net);
    p.inventory.has_the_bomb = static_cast<bool>(has_bomb);

    uint8_t equippedWeapon;
    if (skt.recvall(&equippedWeapon, sizeof(equippedWeapon)) == 0)
        throw std::runtime_error("Error receiving equippedWeapon");
    p.equippedWeapon = static_cast<WeaponType>(equippedWeapon);

    return p;
}

BombData Protocol::recv_bomb_data() {
    uint8_t planted;
    if (skt.recvall(&planted, sizeof(planted)) == 0)
        throw std::runtime_error("Error receiving bomb planted");
    return BombData{static_cast<bool>(planted)};
}

WeaponData Protocol::recv_weapon_data() {
    WeaponData w;
    uint8_t type, weaponName;
    if (skt.recvall(&type, sizeof(type)) == 0 ||
        skt.recvall(&weaponName, sizeof(weaponName)) == 0)
        throw std::runtime_error("Error receiving weapon data");
    w.type = static_cast<WeaponType>(type);
    w.weapon = static_cast<WeaponName>(weaponName);
    return w;
}

Bullet Protocol::deserialize_bullet() {
    Bullet b;
    float values[3];
    for (int i = 0; i < 3; ++i) {
        uint32_t net;
        if (skt.recvall(&net, sizeof(net)) == 0)
            throw std::runtime_error("Error receiving bullet field");
        net = ntohl(net);
        std::memcpy(&values[i], &net, sizeof(float));
    }
    b.target_x = values[0];
    b.target_y = values[1];
    b.angle = values[2];

    uint8_t impact_val;
    if (skt.recvall(&impact_val, sizeof(impact_val)) == 0)
        throw std::runtime_error("Error receiving bullet impact");
    b.impact = static_cast<Impact>(impact_val);

    return b;
}

Shot Protocol::deserialize_shot() {
    Shot s;
    float values[2];
    for (int i = 0; i < 2; ++i) {
        uint32_t net;
        if (skt.recvall(&net, sizeof(net)) == 0)
            throw std::runtime_error("Error receiving shot origin");
        net = ntohl(net);
        std::memcpy(&values[i], &net, sizeof(float));
    }
    s.origin_x = values[0];
    s.origin_y = values[1];

    uint8_t weapon_val;
    if (skt.recvall(&weapon_val, sizeof(weapon_val)) == 0)
        throw std::runtime_error("Error receiving weapon");
    s.weapon = static_cast<WeaponName>(weapon_val);

    uint16_t bullet_count_net;
    if (skt.recvall(&bullet_count_net, sizeof(bullet_count_net)) == 0)
        throw std::runtime_error("Error receiving bullet count");
    uint16_t bullet_count = ntohs(bullet_count_net);

    for (uint16_t i = 0; i < bullet_count; ++i) {
        s.bullets.push_back(deserialize_bullet());
    }

    return s;
}

Entity Protocol::deserialize_entity_player(float x, float y) {
    Entity entity;
    entity.type = EntityType::PLAYER;
    entity.x = x;
    entity.y = y;
    entity.data = recv_player_data();
    return entity;
}

Entity Protocol::deserialize_entity_bomb(float x, float y) {
    Entity entity;
    entity.type = EntityType::BOMB;
    entity.x = x;
    entity.y = y;
    entity.data = recv_bomb_data();
    return entity;
}

Entity Protocol::deserialize_entity_weapon(float x, float y) {
    Entity entity;
    entity.type = EntityType::WEAPON;
    entity.x = x;
    entity.y = y;
    entity.data = recv_weapon_data();
    return entity;
}

Entity Protocol::deserialize_entity() {
    uint8_t type_raw;
    if (skt.recvall(&type_raw, sizeof(type_raw)) == 0)
        throw std::runtime_error("Error receiving entity type");
    EntityType type = static_cast<EntityType>(type_raw);

    uint32_t x_net, y_net;
    if (skt.recvall(&x_net, sizeof(x_net)) == 0 || skt.recvall(&y_net, sizeof(y_net)) == 0)
        throw std::runtime_error("Error receiving entity coordinates");
    x_net = ntohl(x_net);
    y_net = ntohl(y_net);
    float x, y;
    std::memcpy(&x, &x_net, sizeof(float));
    std::memcpy(&y, &y_net, sizeof(float));

    switch (type) {
        case EntityType::PLAYER:
            return deserialize_entity_player(x, y);
        case EntityType::BOMB:
            return deserialize_entity_bomb(x, y);
        case EntityType::WEAPON:
            return deserialize_entity_weapon(x, y);
        default: {
            Entity unknown;
            unknown.type = type;
            unknown.x = x;
            unknown.y = y;
            unknown.data = {};
            return unknown;
        }
    }
}

Response Protocol::deserialize_state() {
    Response r;
    r.type = Type::STATE;
    uint8_t result;
    if (skt.recvall(&result, sizeof(result)) == 0)
        throw std::runtime_error("Error receiving state result");
    r.result = result;

    StateGame state;

    uint8_t phase_val;
    if (skt.recvall(&phase_val, sizeof(phase_val)) == 0)
        throw std::runtime_error("Error receiving phase");
    state.phase = static_cast<Phase>(phase_val);

    uint16_t entity_count_net;
    if (skt.recvall(&entity_count_net, sizeof(entity_count_net)) == 0)
        throw std::runtime_error("Error receiving entity count");
    uint16_t entity_count = ntohs(entity_count_net);

    for (uint16_t i = 0; i < entity_count; ++i) {
        state.entities.push_back(deserialize_entity());
    }

    uint16_t shot_count_net;
    if (skt.recvall(&shot_count_net, sizeof(shot_count_net)) == 0)
        throw std::runtime_error("Error receiving shot count");
    uint16_t shot_count = ntohs(shot_count_net);

    for (uint16_t i = 0; i < shot_count; ++i) {
        state.shots.push(deserialize_shot());
    }

    r.data = std::move(state);
    r.message = deserialize_string();
    return r;
}

Response Protocol::deserialize_state_lobby() {
    Response r;
    r.type = Type::STATE_LOBBY;
    uint8_t result;
    if (skt.recvall(&result, sizeof(result)) == 0) {
        throw std::runtime_error("Error receiving state_lobby result");
    }
    r.result = result;

    StateLobby state;

    uint16_t size_net;
    if (skt.recvall(&size_net, sizeof(size_net)) == 0) {
        throw std::runtime_error("Error receiving state_lobby size");
    }
    uint16_t size = ntohs(size_net);
    state.players.resize(size);

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
        state.players[i].assign(name_buf.begin(), name_buf.end());
    }

    r.data = std::move(state);
    
    r.message = deserialize_string();
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
        case Type::LOBBY_READY:
        case Type::DISCONNECT:
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
    uint32_t id_net;
    int32_t x_net, y_net;

    if (skt.recvall(&id_net, sizeof(id_net)) == 0 ||
        skt.recvall(&x_net, sizeof(x_net)) == 0 ||
        skt.recvall(&y_net, sizeof(y_net)) == 0) {
        throw std::runtime_error("Error receiving MOVE parameters");
    }

    MoveAction move;
    move.id = ntohl(id_net);
    move.vx = ntohl(x_net);
    move.vy = ntohl(y_net);
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

BuyBulletAction Protocol::recv_buy_bullet_action() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving BUY_BULLET parameter");
    }
    return BuyBulletAction{static_cast<WeaponType>(type)};
}

BuyWeaponAction Protocol::recv_buy_weapon_action() {
    uint8_t weaponName;
    if (skt.recvall(&weaponName, sizeof(weaponName)) == 0) {
        throw std::runtime_error("Error receiving BUY_WEAPON parameters");
    }

    BuyWeaponAction action;
    action.weapon = static_cast<WeaponName>(weaponName);
    return action;
}

ChangeWeaponAction Protocol::recv_change_weapon_action() {
    uint8_t type;
    if (skt.recvall(&type, sizeof(type)) == 0) {
        throw std::runtime_error("Error receiving CHANGE_WEAPON parameter");
    }
    return ChangeWeaponAction{static_cast<WeaponType>(type)};
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
        case ActionType::BUY_BULLET:
            action.data = recv_buy_bullet_action();
            break;
        case ActionType::BUY_WEAPON:
            action.data = recv_buy_weapon_action();
            break;
        case ActionType::CHANGE_WEAPON:
            action.data = recv_change_weapon_action();
            break;

        // Acciones sin parámetros
        case ActionType::SHOOT:
        case ActionType::STOP_SHOOTING:
        case ActionType::PLANT:
        case ActionType::STOP_PLANTING:
        case ActionType::DEFUSE:
        case ActionType::STOP_DEFUSING:
        case ActionType::GRAB:
            action.data = {};
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
        case Type::START:
        case Type::DISCONNECT:
            msg.type = type;
            return msg;
        default:
            throw std::runtime_error("Invalid message type");
    }
}

