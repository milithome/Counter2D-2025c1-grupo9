#include "clientReceiverLoop.h"

RecvLoop::RecvLoop(Protocol &proto, Queue<ActionEvent> &q) : protocol(proto), queue(q) {}

void RecvLoop::run() {
    try {
            while (should_keep_running()) {
                Message msg = protocol.recv_message();
                ActionEvent event;
                switch (msg.type)
                {
                case Type::ACTION:
                    event.action = msg.action;
                    queue.try_push(std::move(event));
                    break;
                default:
                    break;
                }
            }
    } catch (const std::exception& e) {
        std::cerr << "RecvLoop exception:: run() " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception random\n";
    }
}

void RecvLoop::stop() {
        Thread::stop();
        queue.close(); //en teoria no se necesita si la cola no es bloqueante
}
