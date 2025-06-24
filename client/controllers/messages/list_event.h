#ifndef LIST_EVENT_H
#define LIST_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>


class ListEvent : public MessageEvent {
public:
    explicit ListEvent() {};

    void send(Protocol& protocol) const override {
        protocol.send_list();
    };

    Type getType() const override { return LIST; };

private:
};

#endif