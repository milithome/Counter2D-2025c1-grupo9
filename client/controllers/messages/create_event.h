#ifndef CREATE_EVENT_H
#define CREATE_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>

class CreateEvent : public MessageEvent {
public:
    explicit CreateEvent(const std::string& partyName) : partyName(partyName) {};

    void send(Protocol& protocol) const override {
        protocol.send_create(partyName);
    };

    Type getType() const override { return CREATE; };

private:
    std::string partyName;
};

#endif