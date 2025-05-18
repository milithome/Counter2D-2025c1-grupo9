#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <functional>
#include "../common/communication/protocol.h"
#include "../common/structures.h"
#include "admin.h"
#include "thread.h"

class ClientHandler : public Thread, public std::enable_shared_from_this<ClientHandler> {
public:
    explicit ClientHandler(Protocol protocol, Admin& admin, std::function<void(std::string, std::shared_ptr<ClientHandler>)> onRegister);
    ClientHandler(Protocol protocol, const std::string& clientName, Admin& admin);

    void run() override;

    ~ClientHandler() override;

    void stop() override;
private:
    Protocol protocol;
    std::string clientName;
    bool active;
    Admin& admin;
    std::function<void(std::string, std::shared_ptr<ClientHandler>)> onRegister;

    void handle_create(const std::string& name);
    void handle_join(const std::string& name);
    void handle_list();
    void handle_game(const std::string& name);
};

#endif