#include "menu_controller.h"
#include <QLayout>
#include <QPushButton>
#include <iostream>
#include "join_event.h"
#include "leave_event.h"
#include "list_event.h"
#include "create_event.h"
#include "start_event.h"

MenuController::MenuController(QtWindow& window, Protocol& protocol) : window(window), protocol(protocol) {
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::listenToMainView(MainView& mainView) {
    QPushButton *searchButton = mainView.getSearchButton();
    QPushButton *createButton = mainView.getCreateButton(); 

    QObject::connect(createButton, &QPushButton::clicked, [this]() {
        onMainViewCreatePartyButtonClicked();
    });
    QObject::connect(searchButton, &QPushButton::clicked, [this]() {
        onMainViewSearchPartyButtonClicked();
    });
}

void MenuController::listenToCreatePartyView(CreatePartyView& createPartyView) {
    QPushButton *backButton = createPartyView.getBackButton();
    QPushButton *createButton = createPartyView.getCreateButton();
    QLineEdit *partyNameTextField = createPartyView.getPartyNameTextField();
    QObject::connect(createButton, &QPushButton::clicked, [this, partyNameTextField]() {
        std::string partyName = partyNameTextField->text().toStdString();
        onCreatePartyViewCreateButtonClicked(partyName);
    });
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onCreatePartyViewBackButtonClicked();
    });
}

void MenuController::listenToSearchPartyView(SearchPartyView& searchPartyView) {
    QPushButton *backButton = searchPartyView.getBackButton();
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onSearchPartyViewBackButtonClicked();
    });
}

void MenuController::listenToPartyView(PartyView& partyView) {
    QPushButton *leaveButton = partyView.getLeaveButton();
    QPushButton *startButton = partyView.getStartButton();
    QObject::connect(leaveButton, &QPushButton::clicked, [this]() {
        onPartyViewLeaveButtonClicked();
    });
    QObject::connect(startButton, &QPushButton::clicked, [this]() {
        onPartyViewStartButtonClicked();
    });
}

void MenuController::onPartyViewLeaveButtonClicked() {
    emit nuevoEvento(new LeaveEvent());
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::onPartyViewStartButtonClicked() {
    emit nuevoEvento(new StartEvent());
    // window.clearWindow();
    // window.quit();
}

void MenuController::onGameStarted() {
    window.clearWindow();
    window.quit();
}

void MenuController::onMainViewCreatePartyButtonClicked() {
    window.clearWindow();
    createPartyView = CreatePartyView();
    listenToCreatePartyView(createPartyView);
    window.showView(createPartyView);
}

void MenuController::onMainViewSearchPartyButtonClicked() {
    emit nuevoEvento(new ListEvent());
    window.clearWindow();
    searchPartyView = SearchPartyView();
    listenToSearchPartyView(searchPartyView);
    window.showView(searchPartyView);
}

void MenuController::onCreatePartyViewCreateButtonClicked(const std::string& partyName) {
    emit nuevoEvento(new CreateEvent(partyName));
    window.clearWindow();
    partyView = PartyView(partyName);
    listenToPartyView(partyView);
    window.showView(partyView);

}

void MenuController::onCreatePartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::onSearchPartyViewJoinButtonClicked(const std::string& partyName) {
    emit nuevoEvento(new JoinEvent(partyName));
    window.clearWindow();
    partyView = PartyView(partyName);
    listenToPartyView(partyView);
    window.showView(partyView);
}

void MenuController::onSearchPartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}



void MenuController::onPartyListReceived(const std::vector<std::string>& parties, const std::string& message, const uint8_t result) {
    for (size_t i = 0; i < parties.size(); i++) {
        searchPartyView.addParty(parties[i]);
    }
    std::unordered_map<std::string, QPushButton *> joinButtons = searchPartyView.getJoinButtons(); 
    for (auto [partyName, button] : joinButtons) {

        QObject::connect(button, &QPushButton::clicked, [this, partyName]() {
            onSearchPartyViewJoinButtonClicked(partyName);
        });
    }
    (void)message;
    (void)result;
}

void MenuController::onLobbyPlayersReceived(const std::vector<std::string>& players, const std::string& message, const uint8_t result) {
    partyView.clearPlayers();
    for (size_t i = 0; i < players.size(); i++) {
        partyView.addPlayer(players[i]);
    }
    (void)message;
    (void)result;
}


void MenuController::onJoinPartyResponseReceived(const std::string& message, const uint8_t result) {
    (void)message;
    (void)result;
}

void MenuController::onCreatePartyResponseReceived(const std::string& message, const uint8_t result) {
    (void)message;
    (void)result;
}

void MenuController::onLeavePartyResponseReceived(const std::string& message, const uint8_t result) {
    (void)message;
    (void)result;
}


void MenuController::onLobbyReady() {
    QPushButton *startButton = partyView.getStartButton();
    startButton->setEnabled(true);
}