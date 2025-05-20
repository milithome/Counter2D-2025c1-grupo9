#ifndef SEND_LOOP_H_
#define SEND_LOOP_H_

#include "server/thread.h"
#include "common/communication/protocol.h"
#include "queue.h"
#include "common/structures.h"
#include <string>
#include "client/controllers/message_event.h"

class SendLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<MessageEvent>& queue;

public:
    SendLoop(Protocol& proto, Queue<MessageEvent>& q);

    void run() override;

    void stop() override;
};

#endif
