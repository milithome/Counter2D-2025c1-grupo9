#include <gtest/gtest.h>
#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "../server/server.h"
#include <thread>


TEST(ProtocolIntegrationTest, SendAndReceiveCreateMessage) {
    Socket listening_socket("12346");

    Socket client_socket("localhost", "12346");

    Socket server_socket = listening_socket.accept();

    Protocol client_protocol(std::move(client_socket));
    Protocol server_protocol(std::move(server_socket));

    // CREATE del lado del cliente 
    std::string game_name = "Partida1";
    client_protocol.send_create(game_name);

    Message msg = server_protocol.recv_message();

    // chequeamos
    EXPECT_EQ(msg.type, Type::CREATE);
    EXPECT_EQ(msg.name, game_name);
}

TEST(ProtocolIntegrationTest, SendAndReceiveJoinMessage) {
    Socket listening_socket("12346");

    Socket client_socket("localhost", "12346");

    Socket server_socket = listening_socket.accept();

    Protocol client_protocol(std::move(client_socket));
    Protocol server_protocol(std::move(server_socket));

    // se crea la partida
    std::string game_name = "Partida1";
    client_protocol.send_create(game_name);

    Message msg = server_protocol.recv_message();

    EXPECT_EQ(msg.type, Type::CREATE);
    EXPECT_EQ(msg.name, game_name);

    // se mete el cliente a la partida
    client_protocol.send_join(game_name);
    Message msg1 = server_protocol.recv_message();

    // chequeamos
    EXPECT_EQ(msg1.type, Type::JOIN);
    EXPECT_EQ(msg1.name, game_name);
}

TEST(ProtocolIntegrationTest, SendAndReceiveListMessage) {
    Socket listening_socket("12346");

    Socket client_socket("localhost", "12346");

    Socket server_socket = listening_socket.accept();

    Protocol client_protocol(std::move(client_socket));
    Protocol server_protocol(std::move(server_socket));

    // se crea la partida
    std::string game_name = "Partida1";
    client_protocol.send_create(game_name);

    Message msg = server_protocol.recv_message();

    std::vector<std::string> partidas_disponibles;
    partidas_disponibles.push_back(game_name);

    // pide que se listen las partidas
    client_protocol.send_list();
    Message list_request = server_protocol.recv_message();
    EXPECT_EQ(list_request.type, Type::LIST);

    server_protocol.send_response({
        Type::LIST,
        0,
        {},
        partidas_disponibles,
        0, // exito
        ""
    });

    Response response = client_protocol.recv_response();

    // chequeamos
    EXPECT_EQ(response.type, Type::LIST);
    ASSERT_EQ(response.partidas.size(), 1);
    EXPECT_EQ(response.partidas[0], "Partida1");
}

/**
TEST(ProtocolIntegrationTest, CreateAndJoinGameBetweenTwoClients) {
    std::string port = "12346";

    Server server(port);
    server.start();

    std::thread client_thread1([port]() {
        Socket client_socket("localhost", port.c_str());
        Protocol client_protocol(std::move(client_socket));
        client_protocol.send_name("aaa");

        std::string game_name = "PartidaTest";
        client_protocol.send_create(game_name);
        Response response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::JOIN);

        auto players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 1);

        players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 2);

        players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 1);

        client_protocol.send_leave_lobby();
        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::LEAVE);
        std::cout << "stop c2" << std::endl;
    });


    std::thread client_thread2([port]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Socket client_socket("localhost", port.c_str());
        Protocol client_protocol(std::move(client_socket));
        client_protocol.send_name("bbb");

        std::string game_name = "PartidaTest";
        client_protocol.send_join(game_name);

        Response response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::JOIN);

        auto players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 2);

        client_protocol.send_leave_lobby();
        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::LEAVE);
        std::cout << "stop c1" << std::endl;
    });


    client_thread1.join();
    client_thread2.join();

    std::istringstream fake_input("q\n");
    std::cin.rdbuf(fake_input.rdbuf());
    std::cout << "stop s" << std::endl;
}
*/

TEST(ProtocolIntegrationTest, CreateAndPlayGameBetweenTwoClients) {
    std::string port = "12345";

    std::thread client_thread1([port]() {
        Socket client_socket("localhost", port.c_str());
        Protocol client_protocol(std::move(client_socket));
        client_protocol.send_name("aaa");

        std::string game_name = "PartidaTest";
        client_protocol.send_create(game_name);
        Response response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::JOIN);

        auto players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 1);

        players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 2);

        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::START);

        auto state = client_protocol.recv_state();
        EXPECT_EQ(state.size(), 2);

        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::FINISH);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    std::thread client_thread2([port]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Socket client_socket("localhost", port.c_str());
        Protocol client_protocol(std::move(client_socket));
        client_protocol.send_name("bbb");

        std::string game_name = "PartidaTest";
        client_protocol.send_join(game_name);

        Response response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::JOIN);

        auto players = client_protocol.recv_state_lobby();
        EXPECT_EQ(players.size(), 2);

        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::START);

        auto state = client_protocol.recv_state();
        EXPECT_EQ(state.size(), 2);

        response = client_protocol.recv_response();
        EXPECT_EQ(response.type, Type::FINISH);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    client_thread1.join();
    client_thread2.join();
}