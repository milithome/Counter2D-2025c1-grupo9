#ifndef CONNECTION_EXCEPTIONS_H
#define CONNECTION_EXCEPTIONS_H

#include <stdexcept>

class ClientClosedConnection : public std::runtime_error {
public:
    ClientClosedConnection() : std::runtime_error("Connection closed orderly by client") {}
};

class ServerClosedConnection : public std::runtime_error {
public:
    ServerClosedConnection() : std::runtime_error("Connection closed orderly by server") {}
};

#endif
