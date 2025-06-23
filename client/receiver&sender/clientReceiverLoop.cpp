#include "clientReceiverLoop.h"
#include <functional>

RecvLoop::RecvLoop(Protocol &proto, Queue<Response> &q) : protocol(proto), queue(q), active(true)  {}

void RecvLoop::run() {
    try {
        while (active) {
            Response msg = protocol.recv_response();
            queue.push(msg);
        }
    } catch (const ClientClosedConnection& e) {
        stop();
        queue.close();
    } catch (const ServerClosedConnection& e) {
        stop();
        queue.close();
    } catch (const std::exception& e) {
        std::cerr << "RecvLoop exception:: run() " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception random\n";
    }
}


void RecvLoop::stop() {
    active = false;
}
