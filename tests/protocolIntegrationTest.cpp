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
        0,
        {},
        partidas_disponibles,
        {}, // exito
        ""
    });

    Response response = client_protocol.recv_response();

    // chequeamos
    EXPECT_EQ(response.type, Type::LIST);
    ASSERT_EQ(response.partidas.size(), 1);
    EXPECT_EQ(response.partidas[0], "Partida1");
}

