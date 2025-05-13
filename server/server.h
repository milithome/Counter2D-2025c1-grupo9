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
    Acceptor acceptor;
    Admin admin;
};

#endif