#ifndef LEAVE_EVENT_H
#define LEAVE_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>

class LeaveEvent : public MessageEvent {
public:
    explicit LeaveEvent() {};

    void send(Protocol& protocol) const override {
        protocol.send_leave_lobby();
    };

    Type getType() const override { return LEAVE; };
private:
};

#endif