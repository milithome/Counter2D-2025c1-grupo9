#include "message_event.h"

MessageEvent::~MessageEvent() { } 

void MessageEvent::send(Protocol& protocol) const {
    (void)protocol;
}

