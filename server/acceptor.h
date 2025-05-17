#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "thread.h"
#include "../common/communication/socket.h"
#include "../common/communication/protocol.h"
#include "clientHandler.h"
#include "admin.h"

class Acceptor : public Thread {
public:
    Acceptor(const std::string& port, Admin& admin, std::vector<std::shared_ptr<ClientHandler>>& handlers);
    
    virtual void run() override;
    
    void stop() override;

    ~Acceptor() override;
private:
    Socket skt;
    std::atomic<bool> active;
    Admin& admin;
    std::vector<std::shared_ptr<ClientHandler>>& handlers;
};

#endif
