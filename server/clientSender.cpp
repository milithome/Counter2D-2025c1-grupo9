#include "clientSender.h"
#include "admin.h"

ClientSender::ClientSender(Protocol& protocol, const std::string& clientName, Admin& admin, std::shared_ptr<Queue<Response>> requests)
    : protocol(protocol), clientName(clientName), active(true), admin(admin), requests(std::move(requests)) {}

ClientSender::~ClientSender() {}

void ClientSender::run() {
    while (active) {
        try
        {
            Response response = requests->pop();
            protocol.send_response(response);

            /*
            if (response.type == Type::DISCONNECT) {
                active = false;
            }
            */
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in ClientSender: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception in ClientSender." << std::endl;
        }
    }
}

void ClientSender::stop() {
    active = false;
    requests->close();
}