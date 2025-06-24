#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include "connectioneExceptions.h"

/*
 * TDA Socket.
 * Por simplificación este TDA se enfocará solamente
 * en sockets IPv4 para TCP.
 * */
class Socket {
private:
    int skt;
    bool closed;
    int stream_status;

    explicit Socket(int skt);

    void chk_skt_or_fail() const;

public:
    Socket(const char* hostname, const char* servname);
    explicit Socket(const char* servname);

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&&);
    Socket& operator=(Socket&&);

    bool has_data(int timeout_ms = 0) const;

    int sendsome(const void* data, unsigned int sz);
    int recvsome(void* data, unsigned int sz);

    int sendall(const void* data, unsigned int sz);
    int recvall(void* data, unsigned int sz);

    Socket accept();

    void shutdown(int how);

    bool is_stream_send_closed() const;
    bool is_stream_recv_closed() const;

    int close();

    ~Socket();
};
#endif
