#ifndef JOIN_EVENT_H
#define JOIN_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>

class JoinEvent : public MessageEvent {
public:
    explicit JoinEvent(const std::string& partyName) : partyName(partyName) {};

    void send(Protocol& protocol) const override {
        protocol.send_join(partyName);
    };

    Type getType() const override { return JOIN; };
private:
    std::string partyName;
};

#endif