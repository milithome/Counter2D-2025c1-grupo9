#ifndef SEND_LOOP_H_
#define SEND_LOOP_H_

#include "common/utilities/thread.h"
#include "common/communication/protocol.h"
#include "common/utilities/queue.h"
#include "common/structures.h"
#include <string>
#include "client/controllers/message_event.h"

class SendLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<std::shared_ptr<MessageEvent>>& queue;

public:
    SendLoop(Protocol& proto, Queue<std::shared_ptr<MessageEvent>>& q);

    void run() override;

    void stop() override;
};

#endif
