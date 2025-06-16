#include <gtest/gtest.h>
#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "../common/structures.h"
#include <thread>
#include <chrono>

void wait_short_time_actions() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(ProtocolClientSender, SendAndReceiveMoveAction) {
    Socket listening_socket("12353");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        ASSERT_EQ(msg.type, Type::ACTION);
        ASSERT_EQ(msg.action.type, ActionType::MOVE);

        const MoveAction& move = std::get<MoveAction>(msg.action.data);
        EXPECT_EQ(move.vx, 1);
        EXPECT_EQ(move.vy, -1);
    });

    wait_short_time_actions();
    Socket client_socket("localhost", "12353");
    Protocol client_protocol(std::move(client_socket));

    MoveAction actionData {1,-1};
    Action action {ActionType::MOVE, actionData};
    client_protocol.send_action(action);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceivePointToAction) {
    Socket listening_socket("12354");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        ASSERT_EQ(msg.type, Type::ACTION);
        ASSERT_EQ(msg.action.type, ActionType::POINT_TO);

        const PointToAction& pt = std::get<PointToAction>(msg.action.data);
        EXPECT_FLOAT_EQ(pt.value, 3.14f);
    });

    wait_short_time_actions();
    Socket client_socket("localhost", "12354");
    Protocol client_protocol(std::move(client_socket));

    PointToAction pt_action{3.14f};
    Action action{ActionType::POINT_TO, pt_action};
    client_protocol.send_action(action);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveBuyBulletAction) {
    Socket listening_socket("12355");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        ASSERT_EQ(msg.type, Type::ACTION);
        ASSERT_EQ(msg.action.type, ActionType::BUY_BULLET);

        const BuyBulletAction& action = std::get<BuyBulletAction>(msg.action.data);
        EXPECT_EQ(action.type, WeaponType::PRIMARY);
    });

    wait_short_time_actions();
    Socket client_socket("localhost", "12355");
    Protocol client_protocol(std::move(client_socket));

    BuyBulletAction bullet_action{WeaponType::PRIMARY};
    Action action{ActionType::BUY_BULLET, bullet_action};
    client_protocol.send_action(action);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveBuyWeaponAction) {
    Socket listening_socket("12356");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        ASSERT_EQ(msg.type, Type::ACTION);
        ASSERT_EQ(msg.action.type, ActionType::BUY_WEAPON);

        const BuyWeaponAction& action = std::get<BuyWeaponAction>(msg.action.data);
        EXPECT_EQ(action.weapon, WeaponName::AK47);
    });

    wait_short_time_actions();
    Socket client_socket("localhost", "12356");
    Protocol client_protocol(std::move(client_socket));

    BuyWeaponAction weapon_action{WeaponName::AK47};
    Action action{ActionType::BUY_WEAPON, weapon_action};
    client_protocol.send_action(action);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveChangeWeaponAction) {
    Socket listening_socket("12357");

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));

        Message msg = server_protocol.recv_message();
        ASSERT_EQ(msg.type, Type::ACTION);
        ASSERT_EQ(msg.action.type, ActionType::CHANGE_WEAPON);

        const ChangeWeaponAction& action = std::get<ChangeWeaponAction>(msg.action.data);
        EXPECT_EQ(action.type, WeaponType::SECONDARY);
    });

    wait_short_time_actions();
    Socket client_socket("localhost", "12357");
    Protocol client_protocol(std::move(client_socket));

    ChangeWeaponAction change_action{WeaponType::SECONDARY};
    Action action{ActionType::CHANGE_WEAPON, change_action};
    client_protocol.send_action(action);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveSimpleActionsWithoutData) {
    std::vector<std::pair<ActionType, uint16_t>> actions = {
        {ActionType::SHOOT, 12358},
        {ActionType::STOP_SHOOTING, 12359},
        {ActionType::PLANT, 12360},
        {ActionType::STOP_PLANTING, 12361},
        {ActionType::DEFUSE, 12362},
        {ActionType::STOP_DEFUSING, 12363},
        {ActionType::GRAB, 12364},
    };

    for (const auto& [type, port] : actions) {
        Socket listening_socket(std::to_string(port).c_str());

        std::thread server_thread([&]() {
            Socket server_socket = listening_socket.accept();
            Protocol server_protocol(std::move(server_socket));

            Message msg = server_protocol.recv_message();
            ASSERT_EQ(msg.type, Type::ACTION);
            ASSERT_EQ(msg.action.type, type);
        });

        wait_short_time_actions();
        Socket client_socket("localhost", std::to_string(port).c_str());
        Protocol client_protocol(std::move(client_socket));

        Action action{type, {}};
        client_protocol.send_action(action);

        server_thread.join();
    }
}