#ifndef RECV_LOOP_H_
#define RECV_LOOP_H_

#include "common/utilities/thread.h"
#include "common/communication/protocol.h"
#include "common/utilities/queue.h"
#include "common/structures.h"
#include <string>
#include "client/controllers/messages/message_event.h"

#include <variant>


class RecvLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<Response>& queue;
    bool active;

public:
    RecvLoop(Protocol& proto, Queue<Response>& q);

    void run() override;

    void stop() override;

    void kill();
};

#endif
