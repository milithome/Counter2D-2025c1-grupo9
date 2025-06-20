#ifndef MESSAGE_EVENT_H
#define MESSAGE_EVENT_H

#include "common/communication/protocol.h"



class MessageEvent {
public:
    virtual ~MessageEvent() {};
    virtual void send(Protocol& protocol) const = 0;
    virtual Type getType() const = 0;
};

#endif 