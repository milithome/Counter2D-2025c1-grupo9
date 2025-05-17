#ifndef LEAVE_EVENT_H
#define LEAVE_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include <vector>
#include <string>

class LeaveEvent : public MessageEvent {
public:
    explicit LeaveEvent();

    void send(Protocol& protocol) const override;

private:
};

#endif