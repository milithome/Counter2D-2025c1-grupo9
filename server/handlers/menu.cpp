#include "menu.h"

Menu::Menu(Admin& admin, std::shared_ptr<Client> client) : admin(admin), client(client), active(true) {}

Menu::Menu(Admin& admin, std::shared_ptr<Client> client, std::shared_ptr<Queue<Message>> requests) : admin(admin), client(client), active(true) {
    client->receiver->changeQueue(requests);
    client->channels.requests = requests;
}

Menu::~Menu() {}


void Menu::run()
{
    try {
        while (active) {
            Message message = client->channels.requests->pop();
            switch (message.type) {
                case Type::CREATE:
                    handle_create(message.name);
                    break;
                case Type::JOIN:
                    handle_join(message.name);
                    break;
                case Type::LIST:
                    handle_list();
                    break;
                case Type::DISCONNECT:
                    admin.removeClient(client->channels.name);
                    active = false;
                    break;
                case  Type::ACTION:
                    // puede pasar que el clientReceiver siga pushenado acciones de la partida pasada
                    break;
                default:
                    std::cerr << "[Menu::run] Unknown message type received. " << client->channels.name << std::endl;
                    break;
            }
        }
        admin.removeMenu(client->channels.name);
    } catch (const ClosedQueue&) {
        //std::cout << "[" << client->channels.name << "]" << " Closing Menu." << std::endl;
        return;
    } catch (...) {
        std::cerr << "Unknown exception in Menu." << std::endl;
    }
}

void Menu::handle_create(std::string& name) {
    try {
        admin.createMatch(name);
        admin.joinMatch(name, client); 
        active = false;
        send_response_successful(Type::CREATE, "Lobby created successfully");
    } catch (const std::exception& e) {
        send_response_error(Type::CREATE, "Error creating lobby: " + std::string(e.what()));
    }
}

void Menu::handle_join(const std::string& name) {
    try {
        admin.joinMatch(name,client);
        active = false;
        send_response_successful(Type::JOIN, "Joined lobby successfully");
    } catch (const std::exception& e) {
        send_response_error(Type::JOIN, "Error joining lobby: " + std::string(e.what()));
    }
}

void Menu::handle_list() {
    try {
        std::vector<std::string> lobbies = admin.listLobbies();
        Response response = {Type::LIST, 0, LobbyList{lobbies}, "Lobbies listed successfully"};
        send_response(response);
    } catch (const std::exception& e) {
        Response response = {Type::LIST, 1, {}, "Error listing lobbies: " + std::string(e.what())};
        send_response(response);
    }
}

void Menu::send_response(const Response& response) {
    client->channels.responses->push(response);
}

void Menu::send_response_successful(Type type, const std::string& msg, uint8_t result) {
    Response response = {type, result, {}, msg};
    client->channels.responses->push(response);
}

void Menu::send_response_error(Type type, const std::string& msg, uint8_t result) {
    Response response = {type, result, {}, msg};
    client->channels.responses->push(response);
}

void Menu::stop() {
    active = false;
    client->channels.requests->close();
}