#include "join_event.h"


JoinEvent::JoinEvent(const std::string& partyName)
    : partyName(partyName) {}

void JoinEvent::send(Protocol& protocol) const {
    protocol.send_join(partyName);
}

