#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"

class Server {
public:
    Server();
    ~Server();

    void start();

private:
   Acceptor acceptor;
};

#endif