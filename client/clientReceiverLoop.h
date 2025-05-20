#ifndef RECV_LOOP_H_
#define RECV_LOOP_H_

#include "server/thread.h"
#include "common/communication/protocol.h"
#include "queue.h"
#include "common/structures.h"
#include <string>
#include "client/controllers/message_event.h"

#include <variant>


class RecvLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<Response>& queue;

public:
    RecvLoop(Protocol& proto, Queue<Response>& q);

    void run() override;

    void stop() override;
};

#endif
