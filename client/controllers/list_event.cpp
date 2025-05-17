#include "list_event.h"

ListEvent::ListEvent() {}

void ListEvent::send(Protocol& protocol) const {
    protocol.send_list();
}