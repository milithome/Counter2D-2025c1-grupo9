#include <gtest/gtest.h>
#include "common/communication/protocol.h"
#include "common/communication/socket.h"
#include "common/structures.h"
#include <thread>
#include <chrono>

void wait_short_time_server_sender() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void run_response_test(uint16_t port, const Response& responseToSend, std::function<void(const Response&)> verifyFn) {
    Socket listening_socket(std::to_string(port).c_str());

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));
        ASSERT_NO_THROW({
            server_protocol.send_response(responseToSend);
        });
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", std::to_string(port).c_str());
    Protocol client_protocol(std::move(client_socket));

    ASSERT_NO_THROW({
        Response received = client_protocol.recv_response();
        ASSERT_EQ(received.type, responseToSend.type);
        ASSERT_EQ(received.result, responseToSend.result);
        ASSERT_EQ(received.message, responseToSend.message);
        verifyFn(received);
    });

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveLobbyListResponse) {
    Response r{Type::LIST, 0, LobbyList{{"Lobby1", "Lobby2"}}, "Success"};
    run_response_test(12370, r, [](const Response& received) {
        auto list = std::get<LobbyList>(received.data);
        ASSERT_EQ(list.lobbies.size(), 2);
        EXPECT_EQ(list.lobbies[0], "Lobby1");
        EXPECT_EQ(list.lobbies[1], "Lobby2");
    });
}

TEST(ProtocolServerSender, SendAndReceiveStateLobbyResponse) {
    Response r{Type::STATE_LOBBY, 0, StateLobby{{"Player1", "Player2"}}, "Lobby State"};
    run_response_test(12371, r, [](const Response& received) {
        auto state = std::get<StateLobby>(received.data);
        ASSERT_EQ(state.players.size(), 2);
        EXPECT_EQ(state.players[0], "Player1");
        EXPECT_EQ(state.players[1], "Player2");
    });
}

TEST(ProtocolServerSender, SendAndReceiveInitialDataResponse) {
    MapData map;
    map.background_path = "background.png";
    map.sprite_path = "sprite.png";
    map.game_map = {
        {CellType::Walkable, CellType::Blocked},
        {CellType::Terrorist, CellType::AntiTerrorist}
    };
    map.tiles_map = {
        {3, 3},
        {4, 4}
    };
    map.legend_tiles = {
        {3, {0, 1}},
        {4, {1, 1}},
    };

    InitialData data;
    data.data = map;
    data.players = {
        {"Alice", PHOENIX, SEAL_FORCE},
        {"Bob", L337_KREW, GERMAN_GSG9}
    };
    data.weaponsInfo = {
        {AK47, 2700, 90},
        {GLOCK, 300, 60}
    };
    data.shop = {
        {AK47, GLOCK},
        100, 50
    };
    data.times = {
        15.0f, 45.0f, 3.0f, 10.0f
    };

    Response r{Type::INITIAL_DATA, 0, data, "Initial Game Data"};

    run_response_test(12372, r, [](const Response& received) {
        ASSERT_EQ(received.type, Type::INITIAL_DATA);
        ASSERT_EQ(received.result, 0);
        ASSERT_EQ(received.message, "Initial Game Data");

        const auto& init = std::get<InitialData>(received.data);

        ASSERT_EQ(init.players.size(), 2);
        EXPECT_EQ(init.players[0].name, "Alice");
        EXPECT_EQ(init.players[0].terroristSkin, PHOENIX);
        EXPECT_EQ(init.players[0].counterTerroristSkin, SEAL_FORCE);

        EXPECT_EQ(init.players[1].name, "Bob");
        EXPECT_EQ(init.players[1].terroristSkin, L337_KREW);
        EXPECT_EQ(init.players[1].counterTerroristSkin, GERMAN_GSG9);

        ASSERT_EQ(init.data.background_path, "background.png");
        ASSERT_EQ(init.data.sprite_path, "sprite.png");

        ASSERT_EQ(init.data.game_map.size(), 2);
        ASSERT_EQ(init.data.game_map[0].size(), 2);
        EXPECT_EQ(init.data.game_map[0][0], CellType::Walkable);
        EXPECT_EQ(init.data.game_map[0][1], CellType::Blocked);
        EXPECT_EQ(init.data.game_map[1][0], CellType::Terrorist);
        EXPECT_EQ(init.data.game_map[1][1], CellType::AntiTerrorist);

        ASSERT_EQ(init.data.tiles_map.size(), 2);
        ASSERT_EQ(init.data.tiles_map[0].size(), 2);
        EXPECT_EQ(init.data.tiles_map[0][0], 3);
        EXPECT_EQ(init.data.tiles_map[0][1], 3);
        ASSERT_EQ(init.data.tiles_map[1].size(), 2);
        EXPECT_EQ(init.data.tiles_map[1][0], 4);
        EXPECT_EQ(init.data.tiles_map[1][1], 4);

        ASSERT_EQ(init.data.legend_tiles.size(), 2);
        const auto& entry3 = init.data.legend_tiles.at(3);
        EXPECT_EQ(entry3.x, 0);
        EXPECT_EQ(entry3.y, 1);

        const auto& entry4 = init.data.legend_tiles.at(4);
        EXPECT_EQ(entry4.x, 1);
        EXPECT_EQ(entry4.y, 1);

        ASSERT_EQ(init.weaponsInfo.size(), 2);
        EXPECT_EQ(init.weaponsInfo[0].name, AK47);
        EXPECT_EQ(init.weaponsInfo[0].price, 2700);
        EXPECT_EQ(init.weaponsInfo[0].maxAmmo, 90);

        EXPECT_EQ(init.weaponsInfo[1].name, GLOCK);
        EXPECT_EQ(init.weaponsInfo[1].price, 300);
        EXPECT_EQ(init.weaponsInfo[1].maxAmmo, 60);

        ASSERT_EQ(init.shop.weapons.size(), 2);
        EXPECT_EQ(init.shop.weapons[0], AK47);
        EXPECT_EQ(init.shop.weapons[1], GLOCK);
        EXPECT_EQ(init.shop.primaryAmmoPrice, 100);
        EXPECT_EQ(init.shop.secondaryAmmoPrice, 50);

        EXPECT_FLOAT_EQ(init.times.purchase_duration, 15.0f);
        EXPECT_FLOAT_EQ(init.times.bomb_duration, 45.0f);
        EXPECT_FLOAT_EQ(init.times.time_to_plant, 3.0f);
        EXPECT_FLOAT_EQ(init.times.time_until_new_round, 10.0f);
    });
}

TEST(ProtocolServerSender, SendAndReceiveFinishResponse) {
    Response r{Type::FINISH, 0, {}, "Game Over"};
    run_response_test(12374, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveCreateResponse) {
    Response r{Type::CREATE, 0, {}, "Lobby Created Successfully"};
    run_response_test(12375, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveJoinResponse) {
    Response r{Type::JOIN, 0, {}, "Joined Lobby Successfully"};
    run_response_test(12376, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveLeaveResponse) {
    Response r{Type::LEAVE, 0, {}, "Left Lobby Successfully"};
    run_response_test(12377, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveLobbyReadyResponse) {
    Response r{Type::LOBBY_READY, 0, {}, "Lobby is Ready"};
    run_response_test(12378, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveNotLobbyReadyResponse) {
    Response r{Type::NOT_LOBBY_READY, 0, {}, "Lobby is Not Ready"};
    run_response_test(12379, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiverStart) {
    Response r{Type::START, 0, {}, "Game Started"};
    run_response_test(12380, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveName) {
    Response r{Type::NAME, 0, {}, "Game Started"};
    run_response_test(12380, r, [](const Response&) {});
}

TEST(ProtocolServerSender, SendAndReceiveStateGameResponse) {
    Socket listening_socket("12379");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Entity player;
        player.type = PLAYER;
        player.x = 10.0f;
        player.y = 20.0f;
        player.data = PlayerData{
            "Carlos", 45.0f, 1500, 10,
            {M3, GLOCK, 10, 20, true}, WeaponType::PRIMARY, true, true
        };

        Entity bomb;
        bomb.type = BOMB;
        bomb.x = 12.0f;
        bomb.y = 22.0f;
        bomb.data = BombData{PLANTED};

        Entity weapon;
        weapon.type = WEAPON;
        weapon.x = 14.0f;
        weapon.y = 24.0f;
        weapon.data = WeaponData{AK47};

        Bullet bullet;
        bullet.target_x = 15.0f;
        bullet.target_y = 15.0f;
        bullet.angle = 0.5f;
        bullet.impact = HUMAN;

        Shot shot;
        shot.origin_x = 5.0f;
        shot.origin_y = 5.0f;
        shot.bullets.push_back(bullet);
        shot.weapon = AK47;

        std::queue<Shot> shots;
        shots.push(shot);

        Rounds rounds;
        rounds.roundsWonTeamA = 2;
        rounds.roundsWonTeamB = 1;
        rounds.currentRound = 4;
        rounds.winner = {'A', BOMB_DEFUSED};

        Response r;
        r.type = Type::STATE;
        r.result = 0;
        r.data = StateGame{PURCHASE, {player, bomb, weapon}, shots, rounds};
        r.message = "Game State";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12379");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    
    ASSERT_EQ(response.type, Type::STATE);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Game State");

    const auto& state = std::get<StateGame>(response.data);
    ASSERT_EQ(state.phase, PURCHASE);
    ASSERT_EQ(state.entities.size(), 3);

    const auto& player_entity = state.entities[0];
    ASSERT_EQ(player_entity.type, PLAYER);
    ASSERT_FLOAT_EQ(player_entity.x, 10.0f);
    ASSERT_FLOAT_EQ(player_entity.y, 20.0f);
    const auto& player_data = std::get<PlayerData>(player_entity.data);
    EXPECT_EQ(player_data.name, "Carlos");
    EXPECT_FLOAT_EQ(player_data.rotation, 45.0f);
    EXPECT_EQ(player_data.money, 1500);
    EXPECT_EQ(player_data.health, 10);
    EXPECT_EQ(player_data.inventory.primary, M3);
    EXPECT_EQ(player_data.inventory.secondary, GLOCK);
    EXPECT_EQ(player_data.inventory.bulletsPrimary, 10);
    EXPECT_EQ(player_data.inventory.bulletsSecondary, 20);
    EXPECT_TRUE(player_data.inventory.has_the_bomb);
    EXPECT_EQ(player_data.equippedWeapon, WeaponType::PRIMARY);
    EXPECT_TRUE(player_data.alive);
    EXPECT_TRUE(player_data.terrorist);

    const auto& bomb_entity = state.entities[1];
    ASSERT_EQ(bomb_entity.type, BOMB);
    ASSERT_FLOAT_EQ(bomb_entity.x, 12.0f);
    ASSERT_FLOAT_EQ(bomb_entity.y, 22.0f);
    const auto& bomb_data = std::get<BombData>(bomb_entity.data);
    EXPECT_EQ(bomb_data.state, PLANTED);

    const auto& weapon_entity = state.entities[2];
    ASSERT_EQ(weapon_entity.type, WEAPON);
    ASSERT_FLOAT_EQ(weapon_entity.x, 14.0f);
    ASSERT_FLOAT_EQ(weapon_entity.y, 24.0f);
    const auto& weapon_data = std::get<WeaponData>(weapon_entity.data);
    EXPECT_EQ(weapon_data.weapon, AK47);

    std::queue<Shot> shots = state.shots;
    ASSERT_FALSE(shots.empty());
    ASSERT_EQ(shots.size(), 1);
    const Shot received_shot = shots.front();
    EXPECT_FLOAT_EQ(received_shot.origin_x, 5.0f);
    EXPECT_FLOAT_EQ(received_shot.origin_y, 5.0f);
    EXPECT_EQ(received_shot.weapon, AK47);

    ASSERT_EQ(received_shot.bullets.size(), 1);
    const Bullet& received_bullet = received_shot.bullets[0];
    EXPECT_FLOAT_EQ(received_bullet.target_x, 15.0f);
    EXPECT_FLOAT_EQ(received_bullet.target_y, 15.0f);
    EXPECT_FLOAT_EQ(received_bullet.angle, 0.5f);
    EXPECT_EQ(received_bullet.impact, HUMAN);

    EXPECT_EQ(state.rounds.roundsWonTeamA, 2);
    EXPECT_EQ(state.rounds.roundsWonTeamB, 1);
    EXPECT_EQ(state.rounds.currentRound, 4);
    EXPECT_EQ(state.rounds.winner.team, 'A');
    EXPECT_EQ(state.rounds.winner.typeEndRound, BOMB_DEFUSED);

    server_thread.join();
}
