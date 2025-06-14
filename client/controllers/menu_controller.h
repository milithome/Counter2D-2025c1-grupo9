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
#include "message_event.h"
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

    QPoint getWindowPosition() { return window.getPosition(); }

    void showStartingScreen() { 
        mainView = MainView();
        listenToMainView(mainView);
        window.showView(mainView);
        window.show();
    }

    

private:
    QtWindow& window;
    MainView mainView;
    SearchPartyView searchPartyView;
    PartyView partyView;
    CreatePartyView createPartyView;
    ConnectToServerView connectToServerView;

    std::string pName;



    void onMainViewCreatePartyButtonClicked();
    void onMainViewSearchPartyButtonClicked();
    void onCreatePartyViewCreateButtonClicked();
    void onCreatePartyViewBackButtonClicked();
    void onSearchPartyViewJoinButtonClicked(const std::string& partyName);
    void onSearchPartyViewBackButtonClicked();
    void onPartyViewLeaveButtonClicked();
    void onPartyViewStartButtonClicked();

    void onConnectViewConnectButtonClicked();
    void onConnectViewBackButtonClicked();

    void listenToMainView(MainView& mainView);
    void listenToSearchPartyView(SearchPartyView& searchPartyView);
    void listenToCreatePartyView(CreatePartyView& createPartyView);
    void listenToPartyView(PartyView& createPartyView);
    void listenToConnectView(ConnectToServerView& connectToServerView);


signals:
    void nuevoEvento(std::shared_ptr<MessageEvent> event);
    void connectRequest(const std::string& name, const std::string& addr, const std::string& port);
};

#endif 