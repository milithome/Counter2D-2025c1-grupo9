#include "start_event.h"

StartEvent::StartEvent() {}

void StartEvent::send(Protocol& protocol) const {
    protocol.send_start();
}
