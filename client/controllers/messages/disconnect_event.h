#ifndef DISCONNECT_EVENT_H
#define DISCONNECT_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>

class DisconnectEvent : public MessageEvent {
public:
    explicit DisconnectEvent() {};

    void send(Protocol& protocol) const override {
        protocol.send_disconnect();
    };

    Type getType() const override { return DISCONNECT; };
};

#endif