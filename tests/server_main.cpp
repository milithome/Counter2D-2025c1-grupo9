#include "server.h"

int main() {
    try {
        Socket accept("12345");
        Socket client = accept.accept();
        Protocol protocol(std::move(client));
        std::cout << "Socket connected" << std::endl;


        Message msg = protocol.recv_message();
        std::cout << "Message created recived" << std::endl;
        Response r = {
            Type::CREATE,
            0,
            {},
            {},
            0, // exito
            ""
        };
        protocol.send_response(r);
        std::cout << "Response sent" << std::endl;

        msg = protocol.recv_message();
        std::cout << "Message list received" << std::endl;
        r = {
            Type::LIST,
            0,
            {},
            {"Lobby1", "Lobby2"},
            0, // exito
            ""
        };
        protocol.send_response(r);
        std::cout << "Response sent" << std::endl;

        msg = protocol.recv_message();
        std::cout << "Message join received" << std::endl;
        r = {
            Type::JOIN,
            0,
            {},
            {},
            0, // exito
            ""
        };
        protocol.send_response(r);
        std::cout << "Response sent" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}