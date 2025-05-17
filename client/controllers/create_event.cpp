#include "create_event.h"

CreateEvent::CreateEvent(const std::string& partyName)
    : partyName(partyName) {}

void CreateEvent::send(Protocol& protocol) const {
    protocol.send_create(partyName);
}