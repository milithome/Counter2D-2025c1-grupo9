#ifndef RECV_LOOP_H_
#define RECV_LOOP_H_

#include "thread.h"
#include "common/communication/protocol.h"
#include "queue.h"
#include "common/structures.h"
#include <string>

class RecvLoop : public Thread {
private:    
    Protocol& protocol;
    Queue<ActionEvent>& queue;

public:
    RecvLoop(Protocol& proto, Queue<ActionEvent>& q);

    void run() override;

    void stop() override;
};

#endif
