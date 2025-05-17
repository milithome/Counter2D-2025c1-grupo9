#include "leave_event.h"

LeaveEvent::LeaveEvent() {}

void LeaveEvent::send(Protocol& protocol) const {
    protocol.send_leave_lobby();
}