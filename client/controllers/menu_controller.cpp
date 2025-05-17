#include "menu_controller.h"
#include <QLayout>
#include <QPushButton>
#include <iostream>
#include "join_event.h"
#include "leave_event.h"
#include "list_event.h"
#include "create_event.h"

MenuController::MenuController(QtWindow& window, Protocol& protocol) : window(window), protocol(protocol) {
    MainView mainView = MainView();
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
    std::unordered_map<std::string, QPushButton *> joinButtons = searchPartyView.getJoinButtons(); 

    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onSearchPartyViewBackButtonClicked();
    });

    for (auto [partyName, button] : joinButtons) {
        QObject::connect(button, &QPushButton::clicked, [this, partyName]() {
            onSearchPartyViewJoinButtonClicked(partyName);
        });
    }
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
    emit nuevoEvento(LeaveEvent());
    window.clearWindow();
    MainView mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::onPartyViewStartButtonClicked() {
    emit partidaIniciada();
    window.clearWindow();
    window.quit();
    // TODO: hacer algo para que empiece la partida?
}

void MenuController::onMainViewCreatePartyButtonClicked() {
    window.clearWindow();
    CreatePartyView createPartyView = CreatePartyView();
    listenToCreatePartyView(createPartyView);
    window.showView(createPartyView);
}

void MenuController::onMainViewSearchPartyButtonClicked() {
    emit nuevoEvento(ListEvent());
    window.clearWindow();
    SearchPartyView searchPartyView = SearchPartyView();
    listenToSearchPartyView(searchPartyView);
    window.showView(searchPartyView);
}

void MenuController::onCreatePartyViewCreateButtonClicked(const std::string& partyName) {
    emit nuevoEvento(CreateEvent(partyName));
    window.clearWindow();
    PartyView partyView = PartyView(partyName);
    listenToPartyView(partyView);
    window.showView(partyView);

}

void MenuController::onCreatePartyViewBackButtonClicked() {
    window.clearWindow();
    MainView mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::onSearchPartyViewJoinButtonClicked(const std::string& partyName) {
    emit nuevoEvento(JoinEvent(partyName));
    window.clearWindow();
    PartyView partyView = PartyView(partyName);
    listenToPartyView(partyView);
    window.showView(partyView);
}

void MenuController::onSearchPartyViewBackButtonClicked() {
    window.clearWindow();
    MainView mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}
