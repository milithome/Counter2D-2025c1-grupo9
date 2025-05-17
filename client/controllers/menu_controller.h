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

private:
    QtWindow& window;
    Protocol& protocol;

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
    void nuevoEvento(const MessageEvent& event);
};

#endif 