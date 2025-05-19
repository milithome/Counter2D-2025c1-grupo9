#ifndef SEND_LOOP_H_
#define SEND_LOOP_H_

#include "thread.h"
#include "common/communication/protocol.h"
#include "queue.h"
#include "common/structures.h"
#include <string>

class SendLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<Action>& queue;

public:
    SendLoop(Protocol& proto, Queue<Action>& q);

    void run() override;

    void stop() override;
};

#endif
