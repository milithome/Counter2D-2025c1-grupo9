#include "admin.h"
#include "match.h"
#include "menu.h"

Admin::Admin(ServerConfig serverConfig) : serverConfig(serverConfig) {}

Admin::~Admin(){}

void Admin::stop() {
    //std::lock_guard<std::mutex> lock(mtx);
    // Paro los hilos
    for (const auto& pair : menus) {
        pair.second->stop();
    }

    // Espero que paren los hilos de los clientes
    for (const auto& client : clients) {
        client->receiver->stop();
        client->sender->stop();
    }

    for (const auto& pair : matches) {
        pair.second->stop();
    }

    // Espero que paren los hilos
    for (const auto& pair : menus) {
        pair.second->join();
    }

    for (const auto& client : clients) {
        client->receiver->join();
        client->sender->join();
    }

    for (const auto& pair : matches) {
        pair.second->join();
    }

    // Limpio los arreglos
    menus.clear();
    clients.clear();
    matches.clear();
    unnamedClients.clear();
}

ServerConfig& Admin::getServerConfig() {
    return serverConfig;
}

std::shared_ptr<Client> Admin::createClient(Protocol&& protocol) {
    std::lock_guard<std::mutex> lock(mtx);
    
    removeFinishedClients();
    removeFinishedMenus();
    removeFinishedMatches();

    auto client = std::make_shared<Client>(std::move(protocol));
    client->channels = ClientChannels{"", std::make_shared<Queue<Message>>(100), std::make_shared<Queue<Response>>(100)};
    
    unnamedClients.insert(client);

    client->receiver = std::make_shared<ClientReceiver>(
        client->protocol,
        client->channels.name,
        *this,
        client->channels.requests,
        [this, client](const std::string& name) {
            if (name == "") {
                throw std::runtime_error("You can't not have a name");
            }
            {
                std::lock_guard<std::mutex> lock(this->mtx);

                auto nameExists = std::any_of(this->clients.begin(), this->clients.end(),
                    [&name](const std::shared_ptr<Client>& other) {
                        return other->channels.name == name;
                    });

                if (nameExists) {
                    throw std::runtime_error("A client with that name already exists");
                }

                client->channels.name = name;
                this->unnamedClients.erase(client);
                this->clients.insert(client);
            }
            client->sender = std::make_shared<ClientSender>(
                client->protocol,
                client->channels.name,
                *this,
                client->channels.responses
            );
            client->sender->start();

            this->createMenu(client);
        }
    );

    client->receiver->start();

    return client;
}

void Admin::removeClient(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find_if(clients.begin(), clients.end(), [&name](const std::shared_ptr<Client>& client) {
        return client->channels.name == name;
    });

    if (it != clients.end()) {
        (*it)->receiver->stop();
        (*it)->sender->stop();
        (*it)->receiver->join();
        (*it)->sender->join();
        clients.erase(it);
    } else {
        std::cerr << "Client not found: " << name << std::endl;
    }
}

void Admin::createMenu(std::shared_ptr<Client> client, bool createNewRequests) {
    std::lock_guard<std::mutex> lock(mtx);
    
    removeFinishedClients();
    removeFinishedMenus();
    removeFinishedMatches();

    if (menus.find(client->channels.name) != menus.end()) {
        throw std::runtime_error("Menu already exists for this client");
    }
    
    if (createNewRequests) {
        auto NewRequests = std::make_shared<Queue<Message>>(100);
        client->receiver->changeQueue(NewRequests);
        client->channels.requests = NewRequests;
    }

    menus[client->channels.name] = std::make_shared<Menu>(*this, client);
    menus[client->channels.name]->start();
}

void Admin::removeMenu(const std::string& clientName) {
    std::lock_guard<std::mutex> lock(mtx);
    completedMenus.push_back(clientName);
}

void Admin::createMatch(std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    
    removeFinishedClients();
    removeFinishedMenus();
    removeFinishedMatches();

    if (name == "") {
        throw std::runtime_error("A game can't be created with a blank name.");
    }

    if (matches.find(name) != matches.end()) {
        throw std::runtime_error("Match already exists with this name");
    }

    auto match = std::make_shared<Match>(name, *this);
    matches[name] = match;
    match->start();
}

void Admin::joinMatch(const std::string& matchName, std::shared_ptr<Client> client) {
    std::lock_guard<std::mutex> lock(mtx);
    
    removeFinishedClients();
    removeFinishedMenus();
    removeFinishedMatches();

    auto it = matches.find(matchName);
    if (it == matches.end()) {
        throw std::runtime_error("Lobby not found");
    }

    it->second->addClient(client);
}

std::vector<std::string> Admin::listLobbies() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> lobbyNames;
    for (const auto& pair : matches) {
        if (pair.second->isInLobby()) {
            lobbyNames.push_back(pair.first);
        }
    }
    return lobbyNames;
}

void Admin::removeMatch(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    completedMatches.push_back(name);
}

void Admin::removeFinishedMatches() {
    for (const auto& name : completedMatches) {
        auto it = matches.find(name);
        if (it != matches.end()) {
            it->second->stop();
            it->second->join();
            matches.erase(it);
        }
    }

    completedMatches.clear();
}

void Admin::removeFinishedMenus() {
    for (const auto& name : completedMenus) {
        auto it = menus.find(name);
        if (it != menus.end()) {
            it->second->stop();
            it->second->join();
            menus.erase(it);
        }
    }
    completedMenus.clear();
}

void Admin::removeFinishedClients() {
    for (const auto& client : clients) {
        if (!client->receiver->is_alive() && !client->sender->is_alive()) {
            client->receiver->stop();
            client->sender->stop();
            client->receiver->join();
            client->sender->join();
        }
    }
}
