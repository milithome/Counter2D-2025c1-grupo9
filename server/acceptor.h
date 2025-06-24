#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "../common/communication/protocol.h"
#include "../common/communication/socket.h"
#include "admin.h"
#include "common/utilities/thread.h"
#include <memory>
#include <unordered_set>

class Acceptor : public Thread {
public:
  Acceptor(const std::string &port, Admin &admin);

  virtual void run() override;

  void stop() override;

  ~Acceptor() override;

private:
  Socket skt;
  std::atomic<bool> active;
  Admin &admin;
};

#endif
