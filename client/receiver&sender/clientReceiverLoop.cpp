#include "clientReceiverLoop.h"
#include <functional>

RecvLoop::RecvLoop(Protocol &proto, Queue<Response> &q) : protocol(proto), queue(q), active(true)  {}

void RecvLoop::run() {
    Response msg;
    while (active) {
        try
        {
            msg = protocol.recv_response();
            queue.push(msg);

            if (msg.type == Type::DISCONNECT) {
                active = false;
            } 
        }
        catch (const std::exception& e) {
            std::cerr << "RecvLoop exception:: run() " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Exception random\n";
        }
            
    }
}

void RecvLoop::stop() {
    active = false;
}

void RecvLoop::kill() {
    protocol.send_disconnect();
    active = false;
}