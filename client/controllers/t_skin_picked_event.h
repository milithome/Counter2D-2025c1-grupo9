#ifndef T_SKIN_PICKED_EVENT_H
#define T_SKIN_PICKED_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include "client/views/game_view.h"
#include <vector>
#include <string>

class TSkinPickedEvent : public MessageEvent {
public:
    explicit TSkinPickedEvent(tSkin skin) : skin(skin) {};

    void send(Protocol& protocol) const override {
        (void)protocol;
    };

    Type getType() const override { return JOIN; };
private:
    tSkin skin;
};

#endif