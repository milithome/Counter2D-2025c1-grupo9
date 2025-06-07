#include <gtest/gtest.h>
#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "../common/structures.h"
#include "../server/server.h"
#include <thread>
#include <chrono>
#include <ext/stdio_filebuf.h>
#include "helper_test.h"

constexpr int N = 2;

TEST(FlowControlTest, TestCreateAndCloseServer) {
    int fds[2];
    pipe(fds);
    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);

    std::thread serverThread([&]() {
        Server server;
        server.start(*input);
    });

    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestServerWithCustomPort) {
    std::string customPort = "12346";
    int fds[2];
    pipe(fds);

    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);

    std::thread serverThread([&]() {
        Server server(customPort);
        server.start(*input);
    });

    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestClientConnectionAndDisconnection) {
    int fds[2];
    pipe(fds);

    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);

    std::thread serverThread([&]() {
        Server server;
        server.start(*input);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    Socket clientSocket("localhost", DEFAULT_PORT);
    Protocol protocol(std::move(clientSocket));

    protocol.send_name("TestClient");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol.send_disconnect();
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::DISCONNECT);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Disconnect successful");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestLobby1ClientResponse) {
    int fds[2];
    pipe(fds);

    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);

    std::thread serverThread([&]() {
        Server server;
        server.start(*input);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    Socket clientSocket("localhost", DEFAULT_PORT);
    Protocol protocol(std::move(clientSocket));

    protocol.send_name("TestClient");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    protocol.send_create("TestLobby");
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::CREATE);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby created successfully");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player TestClient. Current players: 1");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol.send_leave_lobby();
    response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::LEAVE);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Leave successful");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    protocol.send_disconnect();
    response = protocol.recv_response();

    // Ahora apagamos el server
    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestLobby2ClientJoin) {
    int fds[2];
    pipe(fds);

    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);

    std::thread serverThread([&]() {
        Server server;
        server.start(*input);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    Socket clientSocket1("localhost", DEFAULT_PORT);
    Protocol protocol1(std::move(clientSocket1));
    protocol1.send_name("Client1");
    
    Socket clientSocket2("localhost", DEFAULT_PORT);
    Protocol protocol2(std::move(clientSocket2));
    protocol2.send_name("Client2");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // CREO EL LOBBY
    protocol1.send_create("Lobby1");
    Response response = protocol1.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // VERIFICO EL ESTADO DEL LOBBY
    response = protocol1.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client1. Current players: 1");
    
    // CLIENTE 2 SE UNE AL LOBBY
    protocol2.send_join("Lobby1");
    response = protocol2.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // VERIFICO EL ESTADO DEL LOBBY
    response = protocol2.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client2. Current players: 2");

    response = protocol1.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client1. Current players: 2");

    protocol1.send_leave_lobby();
    protocol2.send_leave_lobby();
    response = protocol1.recv_response();
    response = protocol2.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol1.send_disconnect();
    protocol2.send_disconnect();

    // Ahora apagamos el server
    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestLobby3ClientList) {
    int fds[2];
    std::unique_ptr<std::istream> input;
    auto serverThread = start_server(fds, input);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto protocol1 = connect_client("Client1");
    auto protocol2 = connect_client("Client2");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol1.send_create("Lobby1");
    protocol1.recv_response();  // ack
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    Response response = protocol1.recv_response();
    expect_lobby_state(response, "Client1", 1);

    protocol2.send_join("Lobby1");
    protocol2.recv_response();  // ack
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    response = protocol2.recv_response();
    expect_lobby_state(response, "Client2", 2);
    response = protocol1.recv_response();
    expect_lobby_state(response, "Client1", 2);

    auto protocol3 = connect_client("Client3");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol3.send_list();
    response = protocol3.recv_response();
    EXPECT_EQ(response.type, Type::LIST);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobbies listed successfully");

    const LobbyList& lobbyList = std::get<LobbyList>(response.data);
    EXPECT_EQ(lobbyList.lobbies.size(), 1);
    EXPECT_EQ(lobbyList.lobbies[0], "Lobby1");

    protocol1.send_leave_lobby(); protocol1.recv_response();
    protocol2.send_leave_lobby(); protocol2.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<Protocol*> protocols = {&protocol1, &protocol2, &protocol3};
    disconnect_all(protocols);
    shutdown_server(fds[1], serverThread);
}

TEST(FlowControlTest, TestLobbyReadyAndLeaveLobby) {
    int fds[2];
    std::unique_ptr<std::istream> input;
    auto serverThread = start_server(fds, input);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<std::string> clientNames;
    for (int i = 1; i <= N; ++i)
        clientNames.push_back("Client" + std::to_string(i));

    std::vector<std::unique_ptr<Protocol>> clients = create_and_connect_clients(clientNames);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    clients[0]->send_create("GameLobby");
    clients[0]->recv_response(); 

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Response response = clients[0]->recv_response();
    expect_lobby_state(response, clientNames[0], 1);

    for (int i = 1; i < N; ++i) {
        clients[i]->send_join("GameLobby");
        clients[i]->recv_response(); 
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        response = clients[i]->recv_response();
        expect_lobby_state(response, clientNames[i], i + 1);

        for (int j = 0; j < i; ++j) {
            response = clients[j]->recv_response();
            expect_lobby_state(response, clientNames[j], i + 1);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    for (auto& client : clients) {
        response = client->recv_response();
        expect_lobby_ready(response);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    for (auto& client : clients) {
        client->send_leave_lobby();
        client->recv_response();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    disconnect_all(clients);
    shutdown_server(fds[1], serverThread);
}

TEST(FlowControlTest, TestLobbyReadyAndStartGame) {
    int fds[2];
    pipe(fds);
    std::unique_ptr<std::istream> input = make_pipe_istream(fds[0]);
    std::thread serverThread = start_server(*input);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<std::string> clientNames;
    for (int i = 1; i <= N; ++i) {
        clientNames.push_back("Client" + std::to_string(i));
    }

    auto clients = create_and_connect_clients(clientNames);

    Protocol& host = *clients[0];
    create_lobby_and_check_state(host, "GameLobby", clientNames[0], 1);

    for (int i = 1; i < N; ++i) {
        join_lobby_and_check_state(*clients[i], "GameLobby", clientNames[i], i + 1);
        for (int j = 0; j < i; ++j) {
            expect_lobby_state_update(*clients[j], clientNames[j], i + 1);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (auto& client : clients) {
        expect_lobby_ready(*client);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    host.send_start();
    for (auto& client : clients) {
        expect_start_response(*client);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    for (auto& client : clients) {
        expect_initial_data(*client);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (auto& client : clients) {
        expect_game_state(*client);
    }

    disconnect_all(clients);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    shutdown_server(fds[1], serverThread);
}
