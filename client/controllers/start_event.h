#ifndef START_EVENT_H
#define START_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include <vector>
#include <string>


class StartEvent : public MessageEvent {
public:
    explicit StartEvent();

    void send(Protocol& protocol) const override;

private:
};

#endif