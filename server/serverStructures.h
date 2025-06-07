#ifndef SERVER_STRUCTURES_H
#define SERVER_STRUCTURES_H

#include "../common/communication/protocol.h"
#include "clientReceiver.h"
#include "clientSender.h"

struct ClientChannels {
    std::string name;
    std::shared_ptr<Queue<Message>> requests;
    std::shared_ptr<Queue<Response>> responses;
};

struct Client {
    Protocol protocol;
    ClientChannels channels;
    std::shared_ptr<ClientReceiver> receiver;
    std::shared_ptr<ClientSender> sender;

    explicit Client(Protocol&& protocol) : protocol(std::move(protocol)) {}
};

#endif