#include "clientSenderLoop.h"

SendLoop::SendLoop(Protocol &proto, Queue<Action> &q)
        : protocol(proto), queue(q) {}

void SendLoop::run(){
    try {
        while (should_keep_running()) {
            Action msg;
            if (!queue.try_pop(msg)) break;
            protocol.send_action(msg);
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