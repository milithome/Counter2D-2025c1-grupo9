#include "clientReceiverLoop.h"
#include <functional>
#include "common/communication/ServerClosedException.h"

RecvLoop::RecvLoop(Protocol &proto, Queue<Response> &q) : protocol(proto), queue(q), active(true)  {}

void RecvLoop::run() {
    Response msg;
    while (active) {
        try
        {
            msg = protocol.recv_response();
            queue.push(msg);
        }
        catch (const ServerClosedException& e) {
            stop();
            queue.close();
        }
        catch (const std::runtime_error& e) {
            std::cerr << "RecvLoop exception:: run() " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Exception random\n";
        }
            
    }
}

void RecvLoop::stop() {
    active = false;
}
