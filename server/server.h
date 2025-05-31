#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"
#include "admin.h"

#define DEFAULT_PORT "12345"




class Server {
public:
    Server();
    Server(std::string& port);
    ~Server();

    void start(std::istream& input = std::cin);

private:
    Admin admin;
    Acceptor acceptor;
};

#endif