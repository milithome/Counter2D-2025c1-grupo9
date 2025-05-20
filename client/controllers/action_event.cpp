#include "action_event.h"


ActionEvent::ActionEvent(const Action& action)
    : action(action) {}

void ActionEvent::send(Protocol& protocol) const {
    protocol.send_action(action);
}

