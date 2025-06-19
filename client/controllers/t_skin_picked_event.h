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
        protocol.send_action(Action{ActionType::SELECT_T_SKIN, SelectTSkin{skin}});
    };

    Type getType() const override { return ACTION; };
private:
    tSkin skin;
};

#endif