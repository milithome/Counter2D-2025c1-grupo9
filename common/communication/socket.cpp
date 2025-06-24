#include "socket.h"

#include <stdexcept>

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "liberror.h"
#include "resolver.h"


#define STREAM_SEND_CLOSED 0x01
#define STREAM_RECV_CLOSED 0x02
#define STREAM_BOTH_CLOSED 0x03
#define STREAM_BOTH_OPEN 0x00

Socket::Socket(const char* hostname, const char* servname) {
    Resolver resolver(hostname, servname, false);

    int s = -1;
    int skt = -1;
    this->closed = true;
    this->stream_status = STREAM_BOTH_CLOSED;

    while (resolver.has_next()) {
        struct addrinfo* addr = resolver.next();

        if (skt != -1)
            ::close(skt);

        skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (skt == -1) {
            continue;
        }

        s = connect(skt, addr->ai_addr, addr->ai_addrlen);
        if (s == -1) {
            continue;
        }

        this->closed = false;
        this->stream_status = STREAM_BOTH_OPEN;
        this->skt = skt;
        return;
    }

    int saved_errno = errno;

    if (skt != -1)
        ::close(skt);

    throw LibError(saved_errno, "socket construction failed (connect to %s:%s)",
                   (hostname ? hostname : ""), (servname ? servname : ""));
}

Socket::Socket(const char* servname) {
    Resolver resolver(nullptr, servname, true);

    int s = -1;
    int skt = -1;
    this->closed = true;
    this->stream_status = STREAM_BOTH_CLOSED;
    while (resolver.has_next()) {
        struct addrinfo* addr = resolver.next();

        if (skt != -1)
            ::close(skt);

        skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (skt == -1) {
            continue;
        }

        int optval = 1;
        s = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (s == -1) {
            continue;
        }

        s = bind(skt, addr->ai_addr, addr->ai_addrlen);
        if (s == -1) {
            continue;
        }

        s = listen(skt, 20);
        if (s == -1) {
            continue;
        }

        this->closed = false;
        this->stream_status = STREAM_BOTH_OPEN;
        this->skt = skt;
        return;
    }

    int saved_errno = errno;

    if (skt != -1)
        ::close(skt);

    throw LibError(saved_errno, "socket construction failed (listen on %s)",
                   (servname ? servname : ""));
}

Socket::Socket(Socket&& other) {
    this->skt = other.skt;
    this->closed = other.closed;
    this->stream_status = other.closed;

    other.skt = -1;
    other.closed = true;
    other.stream_status = STREAM_BOTH_CLOSED;
}

Socket& Socket::operator=(Socket&& other) {
    if (this == &other)
        return *this;

    if (not this->closed) {
        ::shutdown(this->skt, 2);
        ::close(this->skt);
    }

    this->skt = other.skt;
    this->closed = other.closed;
    other.skt = -1;
    other.closed = true;
    other.stream_status = STREAM_BOTH_CLOSED;

    return *this;
}

bool Socket::has_data(int timeout_ms) const {
    chk_skt_or_fail();

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(this->skt, &read_fds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int s = select(this->skt + 1, &read_fds, nullptr, nullptr, &tv);
    if (s == -1) {
        throw LibError(errno, "socket select failed");
    } else if (s == 0) {
        return false;
    } else {
        return true;
    }
}

int Socket::recvsome(void* data, unsigned int sz) {
    chk_skt_or_fail();
    int s = recv(this->skt, (char*)data, sz, 0);

    if (s == 0) {
        stream_status |= STREAM_RECV_CLOSED;
        if (stream_status & STREAM_SEND_CLOSED) {
            throw ServerClosedConnection();
        } else {
            throw ClientClosedConnection();
        }
    } else if (s == -1) {
        if (errno == ECONNRESET) {
            throw ClientClosedConnection();
        }
        throw LibError(errno, "socket recv failed");
    }

    return s;
}

int Socket::sendsome(const void* data, unsigned int sz) {
    chk_skt_or_fail();
    int s = send(this->skt, (char*)data, sz, MSG_NOSIGNAL);

    if (s == -1) {
        if (errno == EPIPE) {
            stream_status |= STREAM_SEND_CLOSED;
            if (stream_status & STREAM_RECV_CLOSED) {
                throw ClientClosedConnection();
            } else {
                throw ServerClosedConnection();
            }
        }
        throw LibError(errno, "socket send failed");
    }

    return s;
}

int Socket::recvall(void* data, unsigned int sz) {
    unsigned int received = 0;

    while (received < sz) {
        int s = recvsome((char*)data + received, sz - received);

        if (s == 0) {
            if (received == 0)
                throw ServerClosedConnection();
            else
                throw LibError(EPIPE, "socket received only %d of %d bytes", received, sz);
        }

        received += s;
    }

    return sz;
}

int Socket::sendall(const void* data, unsigned int sz) {
    unsigned int sent = 0;

    while (sent < sz) {
        int s = sendsome((char*)data + sent, sz - sent);

        if (s == 0) {
            if (sent == 0)
                return 0;
            throw LibError(EPIPE, "socket sent only %d of %d bytes", sent, sz);
        }

        sent += s;
    }

    return sz;
}

Socket::Socket(int skt) {
    this->skt = skt;
    this->closed = false;
    this->stream_status = STREAM_BOTH_OPEN;
}

Socket Socket::accept() {
    chk_skt_or_fail();
    
    int peer_skt = ::accept(this->skt, nullptr, nullptr);
    if (peer_skt == -1)
        throw LibError(errno, "socket accept failed");

    return Socket(peer_skt);
}

void Socket::shutdown(int how) {
    chk_skt_or_fail();
    if (::shutdown(this->skt, how) == -1) {
        throw LibError(errno, "socket shutdown failed");
    }

    switch (how) {
        case 0:
            stream_status |= STREAM_RECV_CLOSED;
            break;
        case 1:
            stream_status |= STREAM_SEND_CLOSED;
            break;
        case 2:
            stream_status |= STREAM_BOTH_CLOSED;
            break;
        default:
            throw std::runtime_error("Unknow shutdown value");
    }
}

bool Socket::is_stream_send_closed() const { return stream_status & STREAM_SEND_CLOSED; }

bool Socket::is_stream_recv_closed() const { return stream_status & STREAM_RECV_CLOSED; }

int Socket::close() {
    chk_skt_or_fail();
    this->closed = true;
    this->stream_status = STREAM_BOTH_CLOSED;
    return ::close(this->skt);
}

Socket::~Socket() {
    if (not this->closed) {
        ::shutdown(this->skt, 2);
        ::close(this->skt);
    }
}

void Socket::chk_skt_or_fail() const {
    if (skt == -1) {
        throw std::runtime_error("socket with invalid file descriptor (-1), "
                                 "perhaps you are using a *previously moved* "
                                 "socket (and therefore invalid).");
    }
}
