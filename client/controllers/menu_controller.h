#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <string>
#include <functional>
#include "client/views/qtwindow.h"
#include "client/views/main_view.h"
#include "client/views/create_party_view.h"
#include "client/views/connect_to_server_view.h"
#include "client/views/search_party_view.h"
#include "client/views/party_view.h"
#include "messages/message_event.h"
#include "common/communication/protocol.h"

class MenuController : public QWidget {
    Q_OBJECT
public:
    MenuController(QtWindow& window);

    void onPartyListReceived(const std::vector<std::string>& parties, const std::string& message, const uint8_t result);

    void onLobbyPlayersReceived(const std::vector<std::string>& players, const std::string& message, const uint8_t result);

    void onJoinPartyResponseReceived(const std::string& message, const uint8_t result);

    void onCreatePartyResponseReceived(const std::string& message, const uint8_t result);

    void onLeavePartyResponseReceived(const std::string& message, const uint8_t result);

    void onConnectionRequestResponseReceived(const std::string& message, const uint8_t result);

    void onGameStarted();

    void onLobbyReady();
    
    void onLobbyNotReady();

    void onServerClosed();

    QPoint getWindowPosition() { return window.getPosition(); }

    void showStartingScreen() { 
        mainView = new MainView();
        listenToMainView();
        window.showView(mainView);
        window.show();
    }

    

private:
    QtWindow& window;
    MainView *mainView = nullptr;
    SearchPartyView *searchPartyView = nullptr;
    PartyView *partyView = nullptr;
    CreatePartyView *createPartyView = nullptr;
    ConnectToServerView *connectToServerView = nullptr;

    QtView *currentView = nullptr;

    std::string pName; // Esto es un hack para ponerle nombre a la PartyView, hay mejores formas de hacerlo pero no me parece que valga la pena



    void onMainViewCreatePartyButtonClicked();
    void onMainViewSearchPartyButtonClicked();
    void onCreatePartyViewCreateButtonClicked();
    void onCreatePartyViewBackButtonClicked();
    void onSearchPartyViewJoinButtonClicked(const std::string& partyName);
    void onSearchPartyViewBackButtonClicked();
    void onPartyViewLeaveButtonClicked();
    void onPartyViewStartButtonClicked();
    void onConnectViewConnectButtonClicked();

    void listenToMainView();
    void listenToSearchPartyView();
    void listenToCreatePartyView();
    void listenToPartyView();
    void listenToConnectView();


signals:
    void newMessage(std::shared_ptr<MessageEvent> event);
    void connectRequest(const std::string& name, const std::string& addr, const std::string& port);
};

#endif 