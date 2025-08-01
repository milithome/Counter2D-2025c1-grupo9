#ifndef CLIENT_RECEIVER_H
#define CLIENT_RECEIVER_H

#include <functional>
#include "common/communication/protocol.h"
#include "common/structures.h"
#include "common/utilities/thread.h"

class ClientReceiver : public Thread {
    public:
        ClientReceiver(Protocol& protocol, const std::string& clientName, std::shared_ptr<Queue<Message>> requests, std::function<void(const std::string&)> onNameReceived);

        ~ClientReceiver() override;

        void run() override;

        void changeQueue(std::shared_ptr<Queue<Message>> newQueue);

        void stop() override;

    private:
        void handleClientName();
        void handleNameError(const std::string& errorMsg);
        void receiveMessagesLoop();
        void handleDisconnection();

        std::mutex queueMutex;
        Protocol& protocol;
        std::string clientName;
        bool active;
        std::shared_ptr<Queue<Message>> requests;
        std::function<void(const std::string&)> onNameReceived;
};

#endif