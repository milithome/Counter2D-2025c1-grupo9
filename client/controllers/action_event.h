#ifndef ACTION_EVENT_H
#define ACTION_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include <vector>
#include <string>

class ActionEvent : public MessageEvent {
public:
    explicit ActionEvent(const Action& action);

    void send(Protocol& protocol) const override;


private:
    Action action;
};

#endif