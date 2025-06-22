#include "clientReceiverLoop.h"
#include <functional>

RecvLoop::RecvLoop(Protocol &proto, Queue<Response> &q) : protocol(proto), queue(q), active(true)  {}

void RecvLoop::run() {
    try {
        while (active) {
            Response msg = protocol.recv_response();
            queue.push(msg);
        }
    } catch (const std::exception& e) {
        std::cerr << "RecvLoop exception in run(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in RecvLoop::run()." << std::endl;
    }
}


void RecvLoop::stop() {
    active = false;
}
