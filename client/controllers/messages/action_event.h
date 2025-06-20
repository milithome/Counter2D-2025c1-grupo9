#ifndef ACTION_EVENT_H
#define ACTION_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/messages/message_event.h"
#include <vector>
#include <string>

class ActionEvent : public MessageEvent {
public:
    explicit ActionEvent(const Action& action) : action(action) {};

    void send(Protocol& protocol) const override {
        protocol.send_action(action);
    };

    Type getType() const override { return ACTION; };

private:
    Action action;
};

#endif