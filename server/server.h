#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"
#include "admin.h"

#define DEFAULT_PORT "12345"

class Server {
public:
    Server();
    ~Server();

    void start();

private:
    std::vector<std::shared_ptr<ClientHandler>> handlers;
    Admin admin;
    Acceptor acceptor;
};

#endif