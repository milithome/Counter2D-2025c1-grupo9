#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"
#include "admin.h"

class Server {
public:
  explicit Server(const ServerConfig &config);
  ~Server();

  void start(std::istream &input = std::cin);

private:
  Admin admin;
  Acceptor acceptor;
};

#endif