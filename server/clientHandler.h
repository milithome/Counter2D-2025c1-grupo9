#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include "../common/communication/protocol.h"
#include "../common/structures.h"
#include "admin.h"
#include "thread.h"

class ClientHandler : public Thread {
public:
    explicit ClientHandler(Protocol protocol, std::string& clientName, Admin& admin);

    void run() override;

    ~ClientHandler() override;

    void stop() override;
private:
    Protocol protocol;
    std::string clientName;
    bool active;
    Admin& admin;

    void handle_create(const std::string& name);
    bool handle_join(const std::string& name);
    void handle_list();
};

#endif