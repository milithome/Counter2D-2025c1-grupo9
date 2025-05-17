#ifndef LIST_EVENT_H
#define LIST_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include <vector>
#include <string>

class ListEvent : public MessageEvent {
public:
    explicit ListEvent();

    void send(Protocol& protocol) const override;

private:
};

#endif