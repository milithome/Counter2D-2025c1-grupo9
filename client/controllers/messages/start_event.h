#ifndef START_EVENT_H
#define START_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>


class StartEvent : public MessageEvent {
public:
    explicit StartEvent() {};

    void send(Protocol& protocol) const override {
        protocol.send_start();
    };

    Type getType() const override { return START; };
private:
};

#endif