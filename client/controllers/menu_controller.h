#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <string>
#include <functional>
#include "client/views/qtwindow.h"
#include "client/views/main_view.h"
#include "client/views/create_party_view.h"
#include "client/views/search_party_view.h"
#include "client/views/party_view.h"
#include "message_event.h"
#include "common/communication/protocol.h"

class MenuController : public QWidget {
    Q_OBJECT
public:
    MenuController(QtWindow& window, Protocol& protocol);

    // void onEventResponse(std::unique_ptr<MessageEvent> event);

    void onPartyListReceived(const std::vector<std::string>& parties, const std::string& message);

    void onLobbyPlayersReceived(const std::vector<std::string>& players, const std::string& message);

    void onJoinPartyResponseReceived(const std::string& message);

    void onCreatePartyResponseReceived(const std::string& message);

    void onLeavePartyResponseReceived(const std::string& message);


private:
    QtWindow& window;
    Protocol& protocol;
    MainView mainView;
    SearchPartyView searchPartyView;
    PartyView partyView;
    CreatePartyView createPartyView;

    void onMainViewCreatePartyButtonClicked();
    void onMainViewSearchPartyButtonClicked();
    void onCreatePartyViewCreateButtonClicked(const std::string& partyName);
    void onCreatePartyViewBackButtonClicked();
    void onSearchPartyViewJoinButtonClicked(const std::string& partyName);
    void onSearchPartyViewBackButtonClicked();
    void onPartyViewLeaveButtonClicked();
    void onPartyViewStartButtonClicked();

    void listenToMainView(MainView& mainView);
    void listenToSearchPartyView(SearchPartyView& searchPartyView);
    void listenToCreatePartyView(CreatePartyView& createPartyView);
    void listenToPartyView(PartyView& createPartyView);


signals:
    void partidaIniciada();
    void nuevoEvento(std::unique_ptr<MessageEvent> event);
};

#endif 