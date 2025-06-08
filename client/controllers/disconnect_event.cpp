#include "disconnect_event.h"

DisconnectEvent::DisconnectEvent() {}

void DisconnectEvent::send(Protocol& protocol) const {
    protocol.send_disconnect();
}

