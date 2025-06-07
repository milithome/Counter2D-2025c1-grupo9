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
                    send_response_successful(Type::DISCONNECT, "Disconnect successful");
                    admin.removeClient(client->channels.name);
                    active = false;
                    break;
                default:
                    std::cerr << "Unknown message type received." << std::endl;
                    break;
            }
        }
        admin.removeMenu(client->channels.name);
    } catch (const std::exception& e) {
        std::cerr << "Exception in Menu: " << e.what() << std::endl;
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
        if (lobbies.empty()) {
            send_response_successful(Type::LIST, "No lobbies available", 0);
            return;
        }
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
}