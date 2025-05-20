#ifndef MESSAGE_EVENT_H
#define MESSAGE_EVENT_H

#include "common/communication/protocol.h"



class MessageEvent {
public:
    virtual ~MessageEvent() = 0;
    virtual void send(Protocol& protocol) const;

};

#endif 