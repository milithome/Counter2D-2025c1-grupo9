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

std::thread start_server(int fds[2], std::unique_ptr<std::istream>& input) {
    pipe(fds);
    input = make_pipe_istream(fds[0]);

    return std::thread([&]() {
        Server server;
        server.start(*input);
    });
}

Protocol connect_client(const std::string& name) {
    Socket socket("localhost", DEFAULT_PORT);
    Protocol protocol(std::move(socket));
    protocol.send_name(name);
    return protocol;
}

void expect_lobby_state(Response& response, const std::string& client, int player_count) {
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player " + client + ". Current players: " + std::to_string(player_count));
}

void expect_lobby_ready(Response& response) {
    EXPECT_EQ(response.type, Type::LOBBY_READY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby is ready");
}

void disconnect_all(std::vector<Protocol*>& protocols) {
    for (auto* p : protocols) p->send_disconnect();
    for (auto* p : protocols) p->recv_response();
}

void shutdown_server(int write_fd, std::thread& serverThread) {
    write(write_fd, "q\n", 2);
    close(write_fd);
    serverThread.join();
}

void connect_and_send_name(Protocol& protocol, const std::string& name) {
    protocol.send_name(name);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void join_lobby_and_check_state(Protocol& protocol, const std::string& lobbyName,
                                 const std::string& clientName, int expectedPlayers) {
    protocol.send_join(lobbyName);
    Response response = protocol.recv_response(); // Confirmación de JOIN

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    response = protocol.recv_response(); // Estado actualizado del lobby

    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    std::ostringstream expectedMessage;
    expectedMessage << "Lobby state updated for player " << clientName << ". Current players: " << expectedPlayers;
    EXPECT_EQ(response.message, expectedMessage.str());
}

void expect_lobby_state_update(Protocol& protocol, const std::string& clientName, int expectedPlayers) {
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    std::ostringstream expectedMessage;
    expectedMessage << "Lobby state updated for player " << clientName << ". Current players: " << expectedPlayers;
    EXPECT_EQ(response.message, expectedMessage.str());
}

void expect_lobby_ready(Protocol& protocol) {
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::LOBBY_READY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby is ready");
}

void leave_and_disconnect(Protocol& protocol) {
    protocol.send_leave_lobby();
    protocol.recv_response();
    protocol.send_disconnect();
    protocol.recv_response();
}

void disconnect_client(Protocol& protocol) {
    protocol.send_disconnect();
    protocol.recv_response();
}

void expect_start_game(Protocol& protocol) {
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::START);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Game started successfully");
}

void create_lobby_and_check_state(Protocol& protocol, const std::string& lobbyName, const std::string& clientName, int expectedPlayers) {
    protocol.send_create(lobbyName);
    protocol.recv_response(); // confirmación de create
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Response response = protocol.recv_response(); // estado del lobby
    EXPECT_EQ(response.type, Type::STATE_LOBBY);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Lobby state updated for player " + clientName + ". Current players: " + std::to_string(expectedPlayers));
}

void expect_start_response(Protocol& protocol) {
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::START);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Game started");
}

void expect_initial_data(Protocol& protocol) {
    Response response = protocol.recv_response();
    EXPECT_EQ(response.type, Type::INITIAL_DATA);
    EXPECT_EQ(response.result, 0);
    EXPECT_EQ(response.message, "Initial data received successfully");
}

void expect_game_state(Protocol& protocol) {
    try
    {
        Response response = protocol.recv_response();
        EXPECT_EQ(response.type, Type::STATE);
        EXPECT_EQ(response.result, 0);
        EXPECT_EQ(response.message, "Game state received successfully");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}

std::thread start_server(std::istream& input) {
    return std::thread([&]() {
        Server server;
        server.start(input);
    });
}

std::vector<std::unique_ptr<Protocol>> create_and_connect_clients(const std::vector<std::string>& names) {
    std::vector<std::unique_ptr<Protocol>> clients;
    for (const auto& name : names) {
        auto protocol = std::make_unique<Protocol>(Socket("localhost", DEFAULT_PORT));
        connect_and_send_name(*protocol, name);
        clients.push_back(std::move(protocol));
    }
    return clients;
}

void disconnect_all(std::vector<std::unique_ptr<Protocol>>& clients) {
    std::vector<Protocol*> raw_ptrs;
    for (auto& client : clients) {
        raw_ptrs.push_back(client.get());
    }
    disconnect_all(raw_ptrs);
}
