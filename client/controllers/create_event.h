#ifndef CREATE_EVENT_H
#define CREATE_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include <vector>
#include <string>

class CreateEvent : public MessageEvent {
public:
    explicit CreateEvent(const std::string& partyName);

    void send(Protocol& protocol) const override;


private:
    std::string partyName;
};

#endif