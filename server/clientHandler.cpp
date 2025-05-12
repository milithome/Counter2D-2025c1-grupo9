#include "clientHandler.h"

ClientHandler::ClientHandler(Protocol protocol, std::string clientName) : 
    protocol(std::move(protocol)), clientName(clientName), active(true) {}

void ClientHandler::run() {
    while (active) {
        try {
            Message message = protocol.recv_message();
            switch (message.type) {
                case Type::CREATE:
                    // aca deberia crear el hilo lobby
                    break;
                case Type::JOIN:
                    // aca deberia unirme al hilo lobby
                    active = false;
                    break;
                case Type::LIST:
                    // para hacer esto deberia tener manejado el tema de partidas
                    break;
                default:
                    std::cerr << "Unknown message type received." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error receiving message: " << e.what() << std::endl;
            active = false;
        }
    }
     
}