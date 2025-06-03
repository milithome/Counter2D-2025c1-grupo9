#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include "../common/communication/protocol.h"
#include "../common/structures.h"
#include "common/utilities/thread.h"

class Admin;

class ClientSender : public Thread {
public:
    ClientSender(Protocol& protocol, const std::string& clientName, Admin& admin, std::shared_ptr<Queue<Response>> requests);

    ~ClientSender() override;

    void run() override;
    void stop() override;

private:
    Protocol& protocol;
    std::string clientName;
    bool active;
    Admin& admin;
    std::shared_ptr<Queue<Response>> requests;
};


#endif