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

    Utilities::serialize_string(buffer,name);

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending name");
    }
}

void Protocol::send_create(const std::string& name) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::CREATE);

    Utilities::serialize_string(buffer,name);

    if (skt.sendall(buffer.data(), buffer.size()) <= 0) {
        throw std::runtime_error("Error sending CREATE message");
    }
}

void Protocol::send_join(const std::string& name) {
    std::vector<uint8_t> buffer;
    buffer.push_back(Type::JOIN);

    Utilities::serialize_string(buffer,name);

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

    Utilities::serialize_int(buffer, move.vx);
    Utilities::serialize_int(buffer, move.vy);
}

void Protocol::serialize_action_point_to(const Action& action, std::vector<uint8_t>& buffer) {
    const PointToAction& point = std::get<PointToAction>(action.data);

    Utilities::serialize_float(buffer, point.value);
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

void  Protocol::serialize_action_select_tskin(const Action& action, std::vector<uint8_t>& buffer) {
    const SelectTSkin& tskin = std::get<SelectTSkin>(action.data);
    buffer.push_back(static_cast<uint8_t>(tskin.terroristSkin));
}

void  Protocol::serialize_action_select_ctskin(const Action& action, std::vector<uint8_t>& buffer) {
    const SelectCTSkin& ctskin = std::get<SelectCTSkin>(action.data);
    buffer.push_back(static_cast<uint8_t>(ctskin.counterTerroristSkin));
}

void  Protocol::serialize_action_select_map(const Action& action, std::vector<uint8_t>& buffer) {
    const SelectMap& map = std::get<SelectMap>(action.data);
    Utilities::serialize_string(buffer, map.name);
}

void Protocol::send_action(const Action& action) {
    std::vector<uint8_t> buffer;

    buffer.push_back(Type::ACTION);
    buffer.push_back(static_cast<uint8_t>(action.type));

    switch (action.type) {
        case ActionType::SELECT_T_SKIN:
            serialize_action_select_tskin(action, buffer);    
            break;
        case ActionType::SELECT_CT_SKIN:
            serialize_action_select_ctskin(action, buffer);    
            break;
        case ActionType::SELECT_MAP:
            serialize_action_select_map(action, buffer);
            break;
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
    std::vector<uint8_t> buffer = {static_cast<uint8_t>(r.type)};
    
    buffer.push_back(static_cast<uint8_t>(r.result));

    Utilities::serialize_string(buffer, r.message);

    return buffer;
}

std::vector<uint8_t> Protocol::serialize_list(const Response& r) {
    const LobbyList& list = std::get<LobbyList>(r.data);
    
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(r.type));
    buffer.push_back(static_cast<uint8_t>(r.result));

    Utilities::serialize_uint16(buffer,list.lobbies.size());

    for (const auto& partida : list.lobbies) {
        Utilities::serialize_string(buffer, partida);
    }

    Utilities::serialize_string(buffer, r.message);
    return buffer;
}

void Protocol::serialize_player_data(std::vector<uint8_t>& buf, const PlayerData& pdata) {
    Utilities::serialize_string(buf, pdata.name);

    Utilities::serialize_float(buf, pdata.rotation);

    Utilities::serialize_int(buf, pdata.money);

    Utilities::serialize_int(buf, pdata.health);

    /*Inventory*/
    buf.push_back(static_cast<uint8_t>(pdata.inventory.primary));
    buf.push_back(static_cast<uint8_t>(pdata.inventory.secondary));
    Utilities::serialize_uint32(buf, pdata.inventory.bulletsPrimary);
    Utilities::serialize_uint32(buf, pdata.inventory.bulletsSecondary);
    buf.push_back(static_cast<uint8_t>(pdata.inventory.has_the_bomb));


    buf.push_back(static_cast<uint8_t>(pdata.equippedWeapon));
    buf.push_back(static_cast<uint8_t>(pdata.alive));
    buf.push_back(static_cast<uint8_t>(pdata.terrorist));
}

void Protocol::serialize_bomb_data(std::vector<uint8_t>& buf, const BombData& bdata) {
    buf.push_back(static_cast<uint8_t>(bdata.state));
}

void Protocol::serialize_weapon_data(std::vector<uint8_t>& buf, const WeaponData& wdata) {
    buf.push_back(static_cast<uint8_t>(wdata.weapon));
}

void Protocol::serialize_entity(std::vector<uint8_t>& buf, const Entity& entity) {
    buf.push_back(static_cast<uint8_t>(entity.type));

    Utilities::serialize_float(buf, entity.x);
    Utilities::serialize_float(buf, entity.y);

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
    Utilities::serialize_float(buf, b.target_x);
    Utilities::serialize_float(buf, b.target_y);
    Utilities::serialize_float(buf, b.angle);

    buf.push_back(static_cast<uint8_t>(b.impact));
}

void Protocol::serialize_shot(std::vector<uint8_t>& buf, const Shot& s) {
    Utilities::serialize_float(buf, s.origin_x);
    Utilities::serialize_float(buf, s.origin_y);
    Utilities::serialize_uint16(buf, s.bullets.size());

    for (const Bullet& b : s.bullets) {
        serialize_bullet(buf, b);
    }

    buf.push_back(static_cast<uint8_t>(s.weapon));
}

void Protocol::serialize_rounds(std::vector<uint8_t>& buf, const Rounds& r) {
    Utilities::serialize_uint16(buf, r.roundsWonTeamA);
    Utilities::serialize_uint16(buf, r.roundsWonTeamB);
    Utilities::serialize_uint16(buf, r.currentRound);

    buf.push_back(static_cast<uint8_t>(r.winner.team));
    buf.push_back(static_cast<uint8_t>(r.winner.typeEndRound));

}

std::vector<uint8_t> Protocol::serialize_state(const Response& r) {
    const StateGame& game = std::get<StateGame>(r.data);
    std::vector<uint8_t> buf = {
        static_cast<uint8_t>(r.type),
        static_cast<uint8_t>(r.result),
        static_cast<uint8_t>(game.phase)
    };

    Utilities::serialize_uint16(buf, game.entities.size());

    for (const Entity& entity : game.entities) {
        serialize_entity(buf, entity);
    }

    std::queue<Shot> copy = game.shots;
    Utilities::serialize_uint16(buf, copy.size());

    while (!copy.empty()) {
        serialize_shot(buf, copy.front());
        copy.pop();
    }

    serialize_rounds(buf, game.rounds);

    Utilities::serialize_string(buf,r.message);
    
    return buf;
}

std::vector<uint8_t> Protocol::serialize_state_lobby(const Response& r) {
    const StateLobby& stateLobby = std::get<StateLobby>(r.data);

    std::vector<uint8_t> buf = {
        static_cast<uint8_t>(r.type),
        static_cast<uint8_t>(r.result)
    };

    Utilities::serialize_uint16(buf, stateLobby.players.size());

    for (const auto& player : stateLobby.players) {
        Utilities::serialize_string(buf, player);
    }

    Utilities::serialize_string(buf, r.message);
    return buf;
}

/*
void Protocol::serialize_string_vector(const std::vector<std::string>& vec, std::vector<uint8_t>& buf) {
    Utilities::serialize_uint16(buf, vec.size());

    for (const auto& str : vec) {
        Utilities::serialize_string(buf, str);
    }
}
*/

void Protocol::serialize_2d_vector(const std::vector<std::vector<CellType>>& matrix, std::vector<uint8_t>& buf) {
    uint16_t rows = matrix.size();
    uint16_t cols = matrix.empty() ? 0 : matrix[0].size();

    Utilities::serialize_uint16(buf, rows);
    Utilities::serialize_uint16(buf, cols);

    for (const auto& row : matrix) {
        for (CellType val : row) {
            Utilities::serialize_uint16(buf, static_cast<uint16_t>(val));
        }
    }
}

void Protocol::serialize_2d_vector(const std::vector<std::vector<uint16_t>>& matrix, std::vector<uint8_t>& buf) {
    uint16_t rows = matrix.size();
    uint16_t cols = matrix.empty() ? 0 : matrix[0].size();

    Utilities::serialize_uint16(buf, rows);
    Utilities::serialize_uint16(buf, cols);

    for (const auto& row : matrix) {
        for (uint16_t val : row) {
            Utilities::serialize_uint16(buf, val);
        }
    }
}

void Protocol::serialize_legend_entry(const MapLegendEntry& entry, std::vector<uint8_t>& buf) {
    Utilities::serialize_int(buf, entry.x);
    Utilities::serialize_int(buf, entry.y);
}

void Protocol::serialize_legend(const std::unordered_map<uint16_t, MapLegendEntry>& legend, std::vector<uint8_t>& buf) {
    Utilities::serialize_uint16(buf, legend.size());

    for (const auto& [key, entry] : legend) {
        Utilities::serialize_uint16(buf, key);
        serialize_legend_entry(entry, buf);
    }
}

void Protocol::serialize_map_data(const MapData& map, std::vector<uint8_t>& buf) {
    Utilities::serialize_string(buf, map.background_path);
    Utilities::serialize_string(buf, map.sprite_path);

    serialize_2d_vector(map.game_map, buf);
    serialize_2d_vector(map.tiles_map, buf);

    serialize_legend(map.legend_tiles, buf);
}

void Protocol::serialize_playersInfo(const std::vector<PlayerInfo>& players, std::vector<uint8_t>& buf) {
    Utilities::serialize_uint16(buf, players.size());

    for (const auto& playerInfo : players) {
        Utilities::serialize_string(buf, playerInfo.name);
        buf.push_back(static_cast<tSkin>(playerInfo.terroristSkin));
        buf.push_back(static_cast<ctSkin>(playerInfo.counterTerroristSkin));
    }
}

void Protocol::serialize_weaponsInfo(const std::vector<WeaponInfo>& weaponsInfo, std::vector<uint8_t>& buf) {
    Utilities::serialize_uint16(buf, weaponsInfo.size());

    for (const auto& weapon : weaponsInfo) {
        buf.push_back(static_cast<uint8_t>(weapon.name));
        Utilities::serialize_int(buf, weapon.price);
        Utilities::serialize_uint32(buf, weapon.maxAmmo);
    }
}

void Protocol::serialize_shop(const Shop& shop, std::vector<uint8_t>& buf) {
    Utilities::serialize_uint16(buf, shop.weapons.size());

    for (const auto& weapon : shop.weapons) {
        buf.push_back(static_cast<uint8_t>(weapon));
    }

    Utilities::serialize_int(buf, shop.primaryAmmoPrice);
    Utilities::serialize_int(buf, shop.secondaryAmmoPrice);
}

void Protocol::serialize_times(const Times& times, std::vector<uint8_t>& buf) {
    Utilities::serialize_float(buf, times.purchase_duration);
    Utilities::serialize_float(buf, times.bomb_duration);
    Utilities::serialize_float(buf, times.time_to_plant);
    Utilities::serialize_float(buf, times.time_until_new_round);
}

std::vector<uint8_t> Protocol::serialize_initial_data(const Response& r) {
    const InitialData& init = std::get<InitialData>(r.data);

    std::vector<uint8_t> buf = {
        static_cast<uint8_t>(r.type),
        static_cast<uint8_t>(r.result)
    };

    serialize_map_data(init.data, buf);
    serialize_playersInfo(init.players, buf);
    serialize_weaponsInfo(init.weaponsInfo, buf);
    serialize_shop(init.shop, buf);
    serialize_times(init.times, buf);
    

    Utilities::serialize_string(buf, r.message);

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
        case Type::NAME:
        case Type::NOT_LOBBY_READY:
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

    return Utilities::deserialize_string(skt);
}

Response Protocol::deserialize_simple(Type type) {
    Response r;
    r.type = type;
    if (skt.recvall(&r.result, sizeof(r.result)) == 0) {
        throw std::runtime_error("Error receiving result");
    }
    r.data = {};
    r.message = Utilities::deserialize_string(skt);
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
    uint16_t size = Utilities::deserialize_uint16(skt);

    for (uint16_t i = 0; i < size; ++i) {
        list.lobbies.push_back(Utilities::deserialize_string(skt));
    }

    r.data = std::move(list);
    r.message = Utilities::deserialize_string(skt);
    return r;
}

/*
std::vector<std::string> Protocol::deserialize_string_vector() {
    uint16_t count = Utilities::deserialize_uint16(skt);
    std::vector<std::string> result;

    for (uint16_t i = 0; i < count; ++i) {
        result.push_back(Utilities::deserialize_string(skt));
    }

    return result;
}
*/

std::vector<std::vector<CellType>> Protocol::deserialize_celltype_2d_vector() {
    uint16_t rows = Utilities::deserialize_uint16(skt);
    uint16_t cols = Utilities::deserialize_uint16(skt);

    std::vector<std::vector<CellType>> matrix(rows, std::vector<CellType>(cols));

    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            matrix[i][j] = static_cast<CellType>(Utilities::deserialize_uint16(skt));
        }
    }
    return matrix;
}

std::vector<std::vector<uint16_t>> Protocol::deserialize_2d_vector() {
    uint16_t rows = Utilities::deserialize_uint16(skt);
    uint16_t cols = Utilities::deserialize_uint16(skt);
    std::vector<std::vector<uint16_t>> matrix(rows, std::vector<uint16_t>(cols));

    for (uint16_t i = 0; i < rows; ++i) {
        for (uint16_t j = 0; j < cols; ++j) {
            matrix[i][j] = Utilities::deserialize_uint16(skt);
        }
    }
    return matrix;
}

std::unordered_map<uint16_t, MapLegendEntry> Protocol::deserialize_legend() {
    uint16_t count = Utilities::deserialize_uint16(skt);

    std::unordered_map<uint16_t, MapLegendEntry> legend;
    for (uint16_t i = 0; i < count; ++i) {
        uint16_t key = Utilities::deserialize_uint16(skt);
        legend[key] = deserialize_legend_entry();
    }

    return legend;
}

MapLegendEntry Protocol::deserialize_legend_entry() {
    int x = Utilities::deserialize_int(skt);
    int y = Utilities::deserialize_int(skt);

    MapLegendEntry entry;
    entry.x = x;
    entry.y = y;
    return entry;
}

MapData Protocol::deserialize_map_data() {
    MapData map;

    map.background_path = Utilities::deserialize_string(skt);
    map.sprite_path = Utilities::deserialize_string(skt);

    map.game_map = deserialize_celltype_2d_vector();

    map.tiles_map = deserialize_2d_vector();
    map.legend_tiles = deserialize_legend();

    return map;
}

std::vector<PlayerInfo> Protocol::deserialize_playersInfo() {
    uint16_t size = Utilities::deserialize_uint16(skt);
     std::vector<PlayerInfo> result;
    for (uint16_t i = 0; i < size; ++i) {
        std::string name = Utilities::deserialize_string(skt);
        tSkin terroristSkin = static_cast<tSkin>(Utilities::deserialize_uint8(skt));
        ctSkin counterTerroristSkin = static_cast<ctSkin>(Utilities::deserialize_uint8(skt));
        PlayerInfo playerInfo{name,terroristSkin,counterTerroristSkin};
        result.push_back(playerInfo);
    }
    return result;
}

std::vector<WeaponInfo> Protocol::deserialize_weaponsInfo() {
    uint16_t size = Utilities::deserialize_uint16(skt);
    std::vector<WeaponInfo> result;
    for (uint16_t i = 0; i < size; ++i) {
        WeaponName name = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
        int price = Utilities::deserialize_int(skt);
        uint32_t maxAmmo = Utilities::deserialize_uint32(skt);
        result.emplace_back(WeaponInfo{name, price, maxAmmo});
    }
    return result;
}

Shop Protocol::deserialize_shop() {
    uint16_t size = Utilities::deserialize_uint16(skt);
    std::vector<WeaponName> weapons;
    for (uint16_t i = 0; i < size; ++i) {
        WeaponName name = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
        weapons.push_back(name);
    }
    int primaryAmmoPrice = Utilities::deserialize_int(skt);
    int secondaryAmmoPrice = Utilities::deserialize_int(skt);
    return Shop{weapons, primaryAmmoPrice, secondaryAmmoPrice};
}

Times Protocol::deserialize_times() {
    Times times;
    times.purchase_duration = Utilities::deserialize_float(skt);
    times.bomb_duration = Utilities::deserialize_float(skt);
    times.time_to_plant = Utilities::deserialize_float(skt);
    times.time_until_new_round = Utilities::deserialize_float(skt);
    return times;
}

Response Protocol::deserialize_initial_data() {
    Response r;
    r.type = Type::INITIAL_DATA;
    r.result = Utilities::deserialize_uint8(skt);

    InitialData init;
    init.data = deserialize_map_data();
    init.players = deserialize_playersInfo();
    init.weaponsInfo = deserialize_weaponsInfo();
    init.shop = deserialize_shop();
    init.times = deserialize_times();
    
    r.data = std::move(init);
    r.message = Utilities::deserialize_string(skt);
    return r;
}

PlayerData Protocol::recv_player_data() {                
    PlayerData p;

    p.name = Utilities::deserialize_string(skt);
    p.rotation = Utilities::deserialize_float(skt);
    p.money = Utilities::deserialize_int(skt);
    p.health = Utilities::deserialize_int(skt);

    p.inventory.primary = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
    p.inventory.secondary = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
    p.inventory.bulletsPrimary = Utilities::deserialize_uint32(skt);
    p.inventory.bulletsSecondary = Utilities::deserialize_uint32(skt);
    p.inventory.has_the_bomb = static_cast<bool>(Utilities::deserialize_uint8(skt));

    p.equippedWeapon = static_cast<WeaponType>(Utilities::deserialize_uint8(skt));
    p.alive = static_cast<bool>(Utilities::deserialize_uint8(skt));
    p.terrorist = static_cast<bool>(Utilities::deserialize_uint8(skt));

    return p;
}

BombData Protocol::recv_bomb_data() {
    uint8_t planted = Utilities::deserialize_uint8(skt);
    return BombData{static_cast<BombState>(planted)};
}

WeaponData Protocol::recv_weapon_data() {
    WeaponData w;
    w.weapon = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
    return w;
}

Bullet Protocol::deserialize_bullet() {
    Bullet b;
    b.target_x = Utilities::deserialize_float(skt);
    b.target_y = Utilities::deserialize_float(skt);
    b.angle = Utilities::deserialize_float(skt);
    b.impact = static_cast<Impact>(Utilities::deserialize_uint8(skt)); 
    return b;
}

Shot Protocol::deserialize_shot() {
    Shot s;
    s.origin_x = Utilities::deserialize_float(skt);
    s.origin_y = Utilities::deserialize_float(skt);
    uint16_t bullet_count = Utilities::deserialize_uint16(skt);
    for (uint16_t i = 0; i < bullet_count; ++i) {
        s.bullets.push_back(deserialize_bullet());
    }
    s.weapon = static_cast<WeaponName>(Utilities::deserialize_uint8(skt));
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
    EntityType type = static_cast<EntityType>(Utilities::deserialize_uint8(skt));
    float x = Utilities::deserialize_float(skt);
    float y = Utilities::deserialize_float(skt);
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

Rounds Protocol::deserialize_rounds() {
    Rounds rounds;
    rounds.roundsWonTeamA = Utilities::deserialize_uint16(skt);
    rounds.roundsWonTeamB = Utilities::deserialize_uint16(skt);
    rounds.currentRound = Utilities::deserialize_uint16(skt);
    rounds.winner.team = static_cast<char>(Utilities::deserialize_uint16(skt));
    rounds.winner.typeEndRound = static_cast<TypeEndRound>(Utilities::deserialize_uint16(skt));
    return rounds;
}

Response Protocol::deserialize_state() {
    Response r;
    r.type = Type::STATE;
    r.result = Utilities::deserialize_uint8(skt);
    StateGame state;
    uint8_t phase_val = Utilities::deserialize_uint8(skt);
    state.phase = static_cast<Phase>(phase_val);
    uint16_t entity_count = Utilities::deserialize_uint16(skt);
    for (uint16_t i = 0; i < entity_count; ++i) {
        state.entities.push_back(deserialize_entity());
    }
    uint16_t shot_count = Utilities::deserialize_uint16(skt);
    for (uint16_t i = 0; i < shot_count; ++i) {
        state.shots.push(deserialize_shot());
    }

    state.rounds = deserialize_rounds();

    r.data = std::move(state);
    r.message = Utilities::deserialize_string(skt); 
    return r;
}

Response Protocol::deserialize_state_lobby() {
    Response r;
    r.type = Type::STATE_LOBBY;
    r.result = Utilities::deserialize_uint8(skt);
    StateLobby state;
    uint16_t size = Utilities::deserialize_uint16(skt);
    state.players.resize(size);

    for (uint16_t i = 0; i < size; ++i) {
        uint16_t name_size = Utilities::deserialize_uint16(skt);
        std::vector<uint8_t> name_buf(name_size);
        if (skt.recvall(name_buf.data(), name_size) == 0) {
            throw std::runtime_error("Error receiving player name");
        }
        state.players[i].assign(name_buf.begin(), name_buf.end());
    }

    r.data = std::move(state);
    
    r.message = Utilities::deserialize_string(skt);
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
        case Type::NAME:
        case Type::NOT_LOBBY_READY:
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
    msg.name = Utilities::deserialize_string(skt);
    msg.size = static_cast<uint16_t>(msg.name.size());
    return msg;
}

MoveAction Protocol::recv_move_action() {
    MoveAction move;
    move.vx = Utilities::deserialize_int(skt);
    move.vy = Utilities::deserialize_int(skt);
    return move;
}

PointToAction Protocol::recv_point_to_action() {
    float value = Utilities::deserialize_float(skt);
    return PointToAction{value};
}

BuyBulletAction Protocol::recv_buy_bullet_action() {
    uint8_t type = Utilities::deserialize_uint8(skt);
    return BuyBulletAction{static_cast<WeaponType>(type)};
}

BuyWeaponAction Protocol::recv_buy_weapon_action() {
    uint8_t weaponName = Utilities::deserialize_uint8(skt);
    return BuyWeaponAction{static_cast<WeaponName>(weaponName)};
}

ChangeWeaponAction Protocol::recv_change_weapon_action() {
    uint8_t type = Utilities::deserialize_uint8(skt);
    return ChangeWeaponAction{static_cast<WeaponType>(type)};
}

SelectTSkin Protocol::recv_select_tskin_action() {
    tSkin tskin = static_cast<tSkin>(Utilities::deserialize_uint8(skt));
    return SelectTSkin{tskin};
}

SelectCTSkin Protocol::recv_select_ctskin_action() {
    ctSkin ctskin = static_cast<ctSkin>(Utilities::deserialize_uint8(skt));
    return SelectCTSkin{ctskin};
}

SelectMap Protocol::recv_select_map_action() {
    std::string name = Utilities::deserialize_string(skt);
    return SelectMap{name};
}

Message Protocol::deserialize_message_action() { 
    Message msg;
    msg.type = Type::ACTION;

    Action action;
    action.type = static_cast<ActionType>(Utilities::deserialize_uint8(skt));

    switch (action.type) {
        case ActionType::SELECT_T_SKIN:
            action.data = recv_select_tskin_action();
            break;
        case ActionType::SELECT_CT_SKIN:
            action.data = recv_select_ctskin_action();
            break;
        case ActionType::SELECT_MAP:
            action.data = recv_select_map_action();
            break;
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
    Type type = static_cast<Type>(Utilities::deserialize_uint8(skt));
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

