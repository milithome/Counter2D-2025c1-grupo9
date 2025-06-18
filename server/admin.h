#ifndef ADMIN_H
#define ADMIN_H

#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "../common/communication/protocol.h"
#include "serverStructures.h"
#include "../common/utilities/config.h"


class Match;
class Menu;

class Admin {
public:
    explicit Admin(ServerConfig config);
    ~Admin();
 
    void stop();

    ServerConfig& getServerConfig();

    std::shared_ptr<Client> createClient(Protocol&& protocol);
    void removeClient(const std::string& name, bool fromReceiver = false);

    void createMenu(std::shared_ptr<Client> client, bool createNewRequests = false);
    void removeMenu(const std::string& clientName);

    void createMatch(std::string& name);
    void joinMatch(const std::string& matchName, std::shared_ptr<Client> client);
    std::vector<std::string> listLobbies();
    void removeMatch(const std::string& name);
    
private:
    std::mutex mtx;
    ServerConfig serverConfig;

    std::map<std::string, std::shared_ptr<Match>> matches;
    std::map<std::string, std::shared_ptr<Menu>> menus;
    std::unordered_set<std::shared_ptr<Client>> clients;
    std::unordered_set<std::shared_ptr<Client>> unnamedClients;

    std::vector<std::string> completedMatches;
    std::vector<std::string> completedMenus;

    void removeFinishedMatches();
    void removeFinishedClients();
    void removeFinishedMenus();
};

#endif
