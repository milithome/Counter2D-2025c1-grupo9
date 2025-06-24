#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <memory>
#include "common/communication/protocol.h"
#include "common/structures.h"
#include "server/admin.h"
#include "common/utilities/thread.h"
#include "common/utilities/queue.h"
#include "server/serverStructures.h"

class Menu : public Thread {
public:
    Menu(Admin& admin, std::shared_ptr<Client> client);
    Menu(Admin& admin, std::shared_ptr<Client> client, std::shared_ptr<Queue<Message>> requests);

    ~Menu() override;

    void run() override;
    void stop() override;
private:
    Admin& admin;
    std::shared_ptr<Client> client;
    bool active;

    void handle_create(std::string& name);
    void handle_join(const std::string& name);
    void handle_list();
    void send_response(const Response& response);
    void send_response_successful(Type type, const std::string& msg, uint8_t result = 0);
    void send_response_error(Type type, const std::string& msg, uint8_t result = 1);
};

#endif