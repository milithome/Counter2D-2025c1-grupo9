#include <gtest/gtest.h>
#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "../common/structures.h"
#include "../server/server.h"
#include <thread>
#include <chrono>
#include <ext/stdio_filebuf.h>


std::unique_ptr<std::istream> make_pipe_istream(int read_fd) {
    int dup_fd = dup(read_fd);
    auto* filebuf = new __gnu_cxx::stdio_filebuf<char>(dup_fd, std::ios::in);
    return std::make_unique<std::istream>(filebuf);
}

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

    Socket clientSocket3("localhost", DEFAULT_PORT);
    Protocol protocol3(std::move(clientSocket3));
    protocol3.send_name("Client3");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // CLIENTE 3 PIDE LA LISTA DE LOBBIES
    protocol3.send_list();
    response = protocol3.recv_response();
    EXPECT_EQ(response.type, Type::LIST);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobbies listed successfully");
    const LobbyList& lobbyList = std::get<LobbyList>(response.data);
    EXPECT_EQ(lobbyList.lobbies.size(), 1);
    EXPECT_EQ(lobbyList.lobbies[0], "Lobby1");

    protocol1.send_leave_lobby();
    protocol2.send_leave_lobby();
    response = protocol1.recv_response();
    response = protocol2.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    protocol1.send_disconnect();

    protocol2.send_disconnect();

    protocol3.send_disconnect();

    // Ahora apagamos el server
    write(fds[1], "q\n", 2);
    close(fds[1]);

    serverThread.join();
}

TEST(FlowControlTest, TestLobbytReadyAndStartGame) {
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
    
    Socket clientSocket3("localhost", DEFAULT_PORT);
    Protocol protocol3(std::move(clientSocket3));
    protocol3.send_name("Client3");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // CREO EL LOBBY
    protocol1.send_create("GameLobby");
    Response response = protocol1.recv_response();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // VERIFICO EL ESTADO DEL LOBBY
    response = protocol1.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client1. Current players: 1");

    // CLIENTE 2 SE UNE AL LOBBY
    protocol2.send_join("GameLobby");
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

    // CLIENTE 3 SE UNE AL LOBBY
    protocol3.send_join("GameLobby");
    response = protocol3.recv_response();
    
    // VERIFICO EL ESTADO DEL LOBBY
    response = protocol3.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client3. Current players: 3");
    response = protocol1.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client1. Current players: 3");
    response = protocol2.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player Client2. Current players: 3");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // RECIBO QUE EL LOBBY ESTA LISTO
    response = protocol1.recv_response();
    EXPECT_EQ(response.type, Type::LOBBY_READY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby is ready");
    response = protocol2.recv_response();
    EXPECT_EQ(response.type, Type::LOBBY_READY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby is ready");
    response = protocol3.recv_response();
    EXPECT_EQ(response.type, Type::LOBBY_READY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby is ready");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
}