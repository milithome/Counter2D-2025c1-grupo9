#ifndef SERVER_CLOSED_EXCEPTION_H
#define SERVER_CLOSED_EXCEPTION_H
#include <stdexcept>

class ServerClosedException : public std::runtime_error {
public:
    ServerClosedException() : std::runtime_error("Server closed the connection") {}
};
#endif