#include "clientSenderLoop.h"

SendLoop::SendLoop(Protocol &proto, Queue<std::unique_ptr<MessageEvent>> &q)
        : protocol(proto), queue(q) {}

void SendLoop::run(){
    try {
        while (should_keep_running()) {
            std::unique_ptr<MessageEvent> msg;
            if (!queue.try_pop(msg)) break;
            msg->send(protocol);
        }
    } catch (const std::exception& e) {
        std::cerr << "SendLoop exception::run() " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception desconocida\n";
    }
}

void SendLoop::stop() {
    Thread::stop();
    queue.close();
}