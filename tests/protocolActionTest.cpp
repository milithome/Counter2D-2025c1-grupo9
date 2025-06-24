#include <gtest/gtest.h>
#include "common/communication/protocol.h"
#include "common/communication/socket.h"
#include "common/structures.h"
#include <thread>
#include <chrono>

void wait_short_time_actions() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void run_action_test(uint16_t port, const Action& actionToSend, std::function<void(const Message&)> verifyFn) {
    Socket listening_socket(std::to_string(port).c_str());

    std::thread server_thread([&]() {
        Socket server_socket = listening_socket.accept();
        Protocol server_protocol(std::move(server_socket));
        ASSERT_NO_THROW({
            Message msg = server_protocol.recv_message();
            ASSERT_EQ(msg.type, Type::ACTION);
            ASSERT_EQ(msg.action.type, actionToSend.type);
            verifyFn(msg);
        });
    });

    wait_short_time_actions();
    Socket client_socket("localhost", std::to_string(port).c_str());
    Protocol client_protocol(std::move(client_socket));
    client_protocol.send_action(actionToSend);

    server_thread.join();
}

TEST(ProtocolClientSender, SendAndReceiveSelectTSkin) {
    SelectTSkin data{tSkin::GUERRILLA};  
    Action action{ActionType::SELECT_T_SKIN, data};

    run_action_test(12365, action, [](const Message& msg) {
        const auto& skin = std::get<SelectTSkin>(msg.action.data);
        EXPECT_EQ(skin.terroristSkin, tSkin::GUERRILLA);
    });
}

TEST(ProtocolClientSender, SendAndReceiveSelectCTSkin) {
    SelectCTSkin data{ctSkin::SEAL_FORCE};
    Action action{ActionType::SELECT_CT_SKIN, data};

    run_action_test(12366, action, [](const Message& msg) {
        const auto& skin = std::get<SelectCTSkin>(msg.action.data);
        EXPECT_EQ(skin.counterTerroristSkin, ctSkin::SEAL_FORCE);
    });
}

TEST(ProtocolClientSender, SendAndReceiveSelectMap) {
    SelectMap data{"aztec"};
    Action action{ActionType::SELECT_MAP, data};

    run_action_test(12367, action, [](const Message& msg) {
        const auto& map = std::get<SelectMap>(msg.action.data);
        EXPECT_EQ(map.name, "aztec");
    });
}

TEST(ProtocolClientSender, SendAndReceiveMoveAction) {
    MoveAction data{1, -1};
    Action action{ActionType::MOVE, data};

    run_action_test(12353, action, [](const Message& msg) {
        const auto& move = std::get<MoveAction>(msg.action.data);
        EXPECT_EQ(move.vx, 1);
        EXPECT_EQ(move.vy, -1);
    });
}

TEST(ProtocolClientSender, SendAndReceivePointToAction) {
    PointToAction data{3.14f};
    Action action{ActionType::POINT_TO, data};

    run_action_test(12354, action, [](const Message& msg) {
        const auto& pt = std::get<PointToAction>(msg.action.data);
        EXPECT_FLOAT_EQ(pt.value, 3.14f);
    });
}

TEST(ProtocolClientSender, SendAndReceiveBuyBulletAction) {
    BuyBulletAction data{WeaponType::PRIMARY};
    Action action{ActionType::BUY_BULLET, data};

    run_action_test(12355, action, [](const Message& msg) {
        const auto& bullet = std::get<BuyBulletAction>(msg.action.data);
        EXPECT_EQ(bullet.type, WeaponType::PRIMARY);
    });
}

TEST(ProtocolClientSender, SendAndReceiveBuyWeaponAction) {
    BuyWeaponAction data{WeaponName::AK47};
    Action action{ActionType::BUY_WEAPON, data};

    run_action_test(12356, action, [](const Message& msg) {
        const auto& weapon = std::get<BuyWeaponAction>(msg.action.data);
        EXPECT_EQ(weapon.weapon, WeaponName::AK47);
    });
}

TEST(ProtocolClientSender, SendAndReceiveChangeWeaponAction) {
    ChangeWeaponAction data{WeaponType::SECONDARY};
    Action action{ActionType::CHANGE_WEAPON, data};

    run_action_test(12357, action, [](const Message& msg) {
        const auto& change = std::get<ChangeWeaponAction>(msg.action.data);
        EXPECT_EQ(change.type, WeaponType::SECONDARY);
    });
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
        Action action{type, {}};
        run_action_test(port, action, [type](const Message& msg) {
            EXPECT_TRUE(std::holds_alternative<std::monostate>(msg.action.data));
        });
    }
}
