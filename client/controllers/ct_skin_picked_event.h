#ifndef CT_SKIN_PICKED_EVENT_H
#define CT_SKIN_PICKED_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include "client/views/game_view.h"
#include <vector>
#include <string>

class CtSkinPickedEvent : public MessageEvent {
public:
    explicit CtSkinPickedEvent(ctSkin skin) : skin(skin) {};

    void send(Protocol& protocol) const override {
        (void)protocol;
    };

    Type getType() const override { return JOIN; };
private:
    ctSkin skin;
};

#endif