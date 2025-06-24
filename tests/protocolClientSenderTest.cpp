#include <gtest/gtest.h>
#include "common/communication/protocol.h"
#include "common/communication/socket.h"
#include "common/structures.h"
#include <thread>
#include <chrono>

void wait_short_time() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(ProtocolClientSender, SendAndReceiveNameMessage) {
    Socket listening_socket("12353");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        std::string name = server_protocol.recv_name();
        EXPECT_EQ(name, "Jugador1");
    });

    wait_short_time();
    Socket client_socket("localhost", "12353");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_name("Jugador1");

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveListMessage) {
    Socket listening_socket("12349");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        EXPECT_EQ(msg.type, Type::LIST);
    });

    wait_short_time();
    Socket client_socket("localhost", "12349");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_list();

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveCreateMessage) {
    Socket listening_socket("12347");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();

        EXPECT_EQ(msg.type, Type::CREATE);
        EXPECT_EQ(msg.name, "MiPartida");
    });

    wait_short_time();
    Socket client_socket("localhost", "12347");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_create("MiPartida");

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveJoinMessage) {
    Socket listening_socket("12348");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();

        EXPECT_EQ(msg.type, Type::JOIN);
        EXPECT_EQ(msg.name, "Sala123");
    });

    wait_short_time();
    Socket client_socket("localhost", "12348");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_join("Sala123");

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveLeaveMessage) {
    Socket listening_socket("12351");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        EXPECT_EQ(msg.type, Type::LEAVE);
    });

    wait_short_time();
    Socket client_socket("localhost", "12351");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_leave_lobby();

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveStartMessage) {
    Socket listening_socket("12350");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        EXPECT_EQ(msg.type, Type::START);
    });

    wait_short_time();
    Socket client_socket("localhost", "12350");
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_start();

    server_thread.join();
}

