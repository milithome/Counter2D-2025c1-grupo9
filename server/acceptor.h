#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "common/utilities/thread.h"
#include "../common/communication/socket.h"
#include "../common/communication/protocol.h"
#include "admin.h"
#include <unordered_set>
#include <memory>

class Acceptor : public Thread {
public:
    Acceptor(const std::string& port, Admin& admin);
    
    virtual void run() override;
    
    void stop() override;

    ~Acceptor() override;
private:
    Socket skt;
    std::atomic<bool> active;
    Admin& admin;
};

#endif
