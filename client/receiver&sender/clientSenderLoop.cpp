#include "clientSenderLoop.h"

SendLoop::SendLoop(Protocol &proto, Queue<std::shared_ptr<MessageEvent>> &q)
        : protocol(proto), queue(q), active(true) {}

void SendLoop::run(){
    try {
        while (active) {
            std::shared_ptr<MessageEvent> msg = queue.pop();
            msg->send(protocol);
        }
    } catch(const ClosedQueue& e) {
        stop();
    } catch (const ClientClosedConnection& e) {
        stop();
        queue.close();
    } catch (const ServerClosedConnection& e) {
        stop();
        queue.close();
    } catch (const std::exception& e) {
        std::cerr << "SendLoop exception::run() " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception desconocida\n";
    }
}

void SendLoop::stop() {
    active = false;
}
