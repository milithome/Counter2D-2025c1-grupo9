#ifndef MAP_PICKED_EVENT_H
#define MAP_PICKED_EVENT_H

#include "common/communication/protocol.h"
#include "client/controllers/message_event.h"
#include "client/views/game_view.h"
#include <vector>
#include <string>

class MapPickedEvent : public MessageEvent {
public:
    explicit MapPickedEvent(const std::string& map) : map(map) {};

    void send(Protocol& protocol) const override {
        protocol.send_action(Action{ActionType::SELECT_MAP, SelectMap{map}});
    };

    Type getType() const override { return ACTION; };
private:
    std::string map;
};

#endif