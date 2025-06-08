#include "clientReceiverLoop.h"
#include "client/controllers/action_event.h"
#include "client/controllers/join_event.h"
#include "client/controllers/leave_event.h"
#include "client/controllers/list_event.h"
#include "client/controllers/create_event.h"
#include <functional>

RecvLoop::RecvLoop(Protocol &proto, Queue<Response> &q) : protocol(proto), queue(q) {}

void RecvLoop::run() {
    try {
            while (should_keep_running()) {
                Response msg = protocol.recv_response();
                queue.push(msg);
                if (msg.type == DISCONNECT) {
                    stop();
                }
                std::cout << msg.message << std::endl;
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