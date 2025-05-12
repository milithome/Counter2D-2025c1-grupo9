#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <thread.h>
#include <string>
#include "../common/protocol.h"
#include "../common/structures.h"

class ClientHandler : public Thread {
public:
    explicit ClientHandler(Protocol protocol, std::string clientName);

    void run() override;

    ~ClientHandler() override;
private:
    Protocol protocol;
    std::string clientName;
    bool active;
};

#endif