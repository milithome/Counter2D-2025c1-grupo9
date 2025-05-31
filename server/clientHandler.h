#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <functional>
#include <memory>
#include "../common/communication/protocol.h"
#include "../common/structures.h"
#include "admin.h"
#include "common/utilities/thread.h"

class ClientHandler : public Thread, public std::enable_shared_from_this<ClientHandler> {
public:
    ClientHandler(Protocol protocol, Admin& admin,
                  std::function<void(std::string, std::shared_ptr<ClientHandler>)> onRegister);

    ~ClientHandler() override;

    void run() override;
    void stop() override;

private:
    Protocol protocol;
    std::string clientName;
    bool active;
    Admin& admin;
    std::function<void(std::string, std::shared_ptr<ClientHandler>)> onRegister;

    void handle_message(const Message& message);

    void handle_create(const std::string& name);
    void handle_join(const std::string& name);
    void enter_lobby(const std::string& lobbyName, Queue<LobbyRequest>& toLobby, Queue<LobbyRequest>& fromLobby);
    void handle_list();
    void handle_game(const std::string& name);

    bool handle_lobby_client_message(const Message& msg, Queue<LobbyRequest>& toLobby, bool& inLobby);
    bool handle_game_client_message(const Message& msg, Queue<ActionRequest>& toGame, bool& inGame);

    void send_simple_response(Type type, const std::string& msg, uint8_t result);
};

#endif
