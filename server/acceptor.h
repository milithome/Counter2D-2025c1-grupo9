#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "thread.h"
#include "../common/socket.h"
#include "../common/protocol.h"
#include "clientHandler.h"

class Acceptor : public Thread {
public:
    explicit Acceptor(const std::string& port);
    
    virtual void run() override;
    
    void stop() override;
private:
    Socket skt;
    std::atomic<bool> active;
};

#endif
