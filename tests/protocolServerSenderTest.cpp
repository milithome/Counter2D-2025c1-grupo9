#include <gtest/gtest.h>
#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "../common/structures.h"
#include <thread>
#include <chrono>

void wait_short_time_server_sender() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(ProtocolServerSender, SendAndReceiveLobbyListResponse) {
    Socket listening_socket("12370");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::LIST;
        r.result = 0;
        r.data = LobbyList{{"Lobby1", "Lobby2"}};
        r.message = "Success";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12370");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::LIST);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Success");

    auto list = std::get<LobbyList>(response.data);
    ASSERT_EQ(list.lobbies.size(), 2);
    EXPECT_EQ(list.lobbies[0], "Lobby1");
    EXPECT_EQ(list.lobbies[1], "Lobby2");

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveStateLobbyResponse) {
    Socket listening_socket("12371");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::STATE_LOBBY;
        r.result = 0;
        r.data = StateLobby{{"Player1", "Player2"}};
        r.message = "Lobby State";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12371");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::STATE_LOBBY);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Lobby State");

    auto state = std::get<StateLobby>(response.data);
    ASSERT_EQ(state.players.size(), 2);
    EXPECT_EQ(state.players[0], "Player1");
    EXPECT_EQ(state.players[1], "Player2");

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveInitialDataResponse) {
    Socket listening_socket("12372");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        MapData map;
        map.background_path = "background.png";
        map.sprite_path = "sprite.png";
        map.game_map = {{1, 1}, {2, 2}};
        map.legend_game = {
            {1, {0, 0}},
            {2, {1, 0}},
        };

        map.tiles_map = {{3, 3}, {4, 4}};
        map.legend_tiles = {
            {3, {0, 1}},
            {4, {1, 1}},
        };

        Response r;
        r.type = Type::INITIAL_DATA;
        r.result = 0;
        r.data = InitialData{map, {"Alice", "Bob"}};
        r.message = "Initial Game Data";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12372");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();

    ASSERT_EQ(response.type, Type::INITIAL_DATA);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Initial Game Data");

    auto init = std::get<InitialData>(response.data);
    ASSERT_EQ(init.players.size(), 2);
    EXPECT_EQ(init.players[0], "Alice");
    EXPECT_EQ(init.players[1], "Bob");
    
    ASSERT_EQ(init.data.background_path, "background.png");
    ASSERT_EQ(init.data.sprite_path, "sprite.png");
    ASSERT_EQ(init.data.game_map.size(), 2);
    ASSERT_EQ(init.data.legend_game.size(), 2);
    ASSERT_EQ(init.data.tiles_map.size(), 2);
    ASSERT_EQ(init.data.legend_tiles.size(), 2);

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveStateGameResponse) {
    Socket listening_socket("12373");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Entity player;
        player.type = PLAYER;
        player.x = 10.0f;
        player.y = 20.0f;
        player.data = PlayerData{"Carlos", 45.0f, 3, 1500, 100.0f, {M3, GLOCK, 10, 20}};

        Entity bomb;
        bomb.type = BOMB;
        bomb.x = 12.0f;
        bomb.y = 22.0f;
        bomb.data = BombData{true};
        
        Entity weapon;
        weapon.type = WEAPON;
        weapon.x = 14.0f;
        weapon.y = 24.0f;
        weapon.data = WeaponData{WeaponType::PRIMARY, AK47};

        std::vector<Entity> entities = {player, bomb, weapon};

        Bullet b;
        b.origin_x = 5.0f;
        b.origin_y = 5.0f;
        b.target_x = 15.0f;
        b.target_y = 15.0f;
        b.angle = 0.5f;

        std::queue<Bullet> bullets;
        bullets.push(b);

        Response r;
        r.type = Type::STATE;
        r.result = 0;
        r.data = StateGame{PURCHASE, entities, bullets};
        r.message = "Game State";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12373");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::STATE);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Game State");

    auto state = std::get<StateGame>(response.data);
    ASSERT_EQ(state.phase, PURCHASE);
    ASSERT_EQ(state.entities.size(), 3);

    auto entity = state.entities[0];
    ASSERT_EQ(entity.type, PLAYER);
    ASSERT_FLOAT_EQ(entity.x, 10.0f);
    ASSERT_FLOAT_EQ(entity.y, 20.0f);

    auto player = std::get<PlayerData>(state.entities[0].data);
    EXPECT_EQ(player.name, "Carlos");
    EXPECT_FLOAT_EQ(player.rotation, 45.0f);
    EXPECT_EQ(player.lastMoveId, 3);
    EXPECT_EQ(player.money, 1500);
    EXPECT_FLOAT_EQ(player.health, 100.0f);
    EXPECT_EQ(player.inventory.primary, M3);
    EXPECT_EQ(player.inventory.secondary, GLOCK);
    EXPECT_EQ(player.inventory.bulletsPrimary, 10);
    EXPECT_EQ(player.inventory.bulletsSecondary, 20);

    entity = state.entities[1];
    ASSERT_EQ(entity.type, BOMB);
    ASSERT_FLOAT_EQ(entity.x, 12.0f);
    ASSERT_FLOAT_EQ(entity.y, 22.0f);
    auto bomb = std::get<BombData>(state.entities[1].data);
    EXPECT_TRUE(bomb.planted);

    entity = state.entities[2]; 
    ASSERT_EQ(entity.type, WEAPON);
    ASSERT_FLOAT_EQ(entity.x, 14.0f);
    ASSERT_FLOAT_EQ(entity.y, 24.0f);
    auto weapon = std::get<WeaponData>(state.entities[2].data);
    EXPECT_EQ(weapon.type, WeaponType::PRIMARY);
    EXPECT_EQ(weapon.weapon, AK47);

    ASSERT_EQ(state.bullets.size(), 1);

    // obtengo la cola de balas y verifico la primera
    std::queue<Bullet>& bullets = state.bullets;
    ASSERT_FALSE(bullets.empty());
    ASSERT_EQ(bullets.size(), 1);

    Bullet bullet = bullets.front();
    EXPECT_FLOAT_EQ(bullet.origin_x, 5.0f);
    EXPECT_FLOAT_EQ(bullet.origin_y, 5.0f);
    EXPECT_FLOAT_EQ(bullet.target_x, 15.0f);
    EXPECT_FLOAT_EQ(bullet.target_y, 15.0f);
    EXPECT_FLOAT_EQ(bullet.angle, 0.5f);

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveFinishResponse) {
    Socket listening_socket("12374");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::FINISH;
        r.result = 0;
        r.message = "Game Over";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12374");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::FINISH);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Game Over");

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveCreateResponse) {
    Socket listening_socket("12374");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::CREATE;
        r.result = 0;
        r.message = "Lobby Created Successfully";

        server_protocol.send_response(r);
    });

    wait_short_time_server_sender();
    Socket client_socket("localhost", "12374");
    Protocol client_protocol(std::move(client_socket));

    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::CREATE);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Lobby Created Successfully");

    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveJoinResponse) {
    Socket listening_socket("12375");
    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::JOIN;
        r.result = 0;
        r.message = "Joined Lobby Successfully";

        server_protocol.send_response(r);
    });
    wait_short_time_server_sender();
    Socket client_socket("localhost", "12375");
    Protocol client_protocol(std::move(client_socket));
    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::JOIN);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Joined Lobby Successfully");
    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveLeaveResponse) {
    Socket listening_socket("12376");
    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::LEAVE;
        r.result = 0;
        r.message = "Left Lobby Successfully";

        server_protocol.send_response(r);
    });
    wait_short_time_server_sender();
    Socket client_socket("localhost", "12376");
    Protocol client_protocol(std::move(client_socket));
    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::LEAVE);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Left Lobby Successfully");
    server_thread.join();
}

TEST(ProtocolServerSender, SendAndReceiveLobbyReadyResponse) {
    Socket listening_socket("12377");
    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Response r;
        r.type = Type::LOBBY_READY;
        r.result = 0;
        r.message = "Lobby is Ready";

        server_protocol.send_response(r);
    });
    wait_short_time_server_sender();
    Socket client_socket("localhost", "12377");
    Protocol client_protocol(std::move(client_socket));
    Response response = client_protocol.recv_response();
    ASSERT_EQ(response.type, Type::LOBBY_READY);
    ASSERT_EQ(response.result, 0);
    ASSERT_EQ(response.message, "Lobby is Ready");
    server_thread.join();   
}

