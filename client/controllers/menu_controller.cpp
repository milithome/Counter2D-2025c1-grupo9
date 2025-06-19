#include "menu_controller.h"
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <iostream>
#include "join_event.h"
#include "leave_event.h"
#include "list_event.h"
#include "create_event.h"
#include "start_event.h"
#include "t_skin_picked_event.h"
#include "ct_skin_picked_event.h"

MenuController::MenuController(QtWindow& window) : QWidget(nullptr), window(window) {
    connectToServerView = ConnectToServerView();
    listenToConnectView(connectToServerView);
    window.showView(connectToServerView);
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
    QObject::connect(createButton, &QPushButton::clicked, [this]() {
        onCreatePartyViewCreateButtonClicked();
    });
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onCreatePartyViewBackButtonClicked();
    });
}

void MenuController::listenToConnectView(ConnectToServerView& connectView) {
    QPushButton *backButton = connectView.getBackButton();
    QPushButton *connectButton = connectView.getConnectButton();
    QObject::connect(connectButton, &QPushButton::clicked, [this]() {
        onConnectViewConnectButtonClicked();
    });
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onConnectViewBackButtonClicked();
    });
}


void MenuController::listenToSearchPartyView(SearchPartyView& searchPartyView) {
    QPushButton *backButton = searchPartyView.getBackButton();
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onSearchPartyViewBackButtonClicked();
    });

    std::unordered_map<std::string, QPushButton *> joinButtons = searchPartyView.getJoinButtons(); 
    for (auto [partyName, button] : joinButtons) {

        QObject::connect(button, &QPushButton::clicked, [this, partyName]() {
            onSearchPartyViewJoinButtonClicked(partyName);
            pName = partyName;
        });
    }
}

void MenuController::listenToPartyView(PartyView& partyView) {
    QPushButton *leaveButton = partyView.getLeaveButton();
    QPushButton *startButton = partyView.getStartButton();
    QPushButton *settingsButton = partyView.getSettingsButton();
    QObject::connect(leaveButton, &QPushButton::clicked, [this]() {
        onPartyViewLeaveButtonClicked();
    });
    QObject::connect(startButton, &QPushButton::clicked, [this]() {
        onPartyViewStartButtonClicked();
    });
    QObject::connect(settingsButton, &QPushButton::clicked, [this, &partyView]() {
        partyView.showModal();
    });
    std::unordered_map<tSkin, QPushButton*> tSkins = partyView.getTSkinButtons();
    std::unordered_map<ctSkin, QPushButton*> ctSkins = partyView.getCtSkinButtons();

    for (auto [skin, button] : tSkins) {
        QObject::connect(button, &QPushButton::clicked, [this, &skin]() {
            emit nuevoEvento(std::make_shared<TSkinPickedEvent>(skin));
        });
    }
    for (auto [skin, button] : ctSkins) {
        QObject::connect(button, &QPushButton::clicked, [this, &skin]() {
            emit nuevoEvento(std::make_shared<CtSkinPickedEvent>(skin));
        });
    }
}

void MenuController::onPartyViewLeaveButtonClicked() {
    emit nuevoEvento(std::make_shared<LeaveEvent>());
}

void MenuController::onPartyViewStartButtonClicked() {
    emit nuevoEvento(std::make_shared<StartEvent>());
}

void MenuController::onGameStarted() {
    window.clearWindow();
    // window.quit();
}

void MenuController::onMainViewCreatePartyButtonClicked() {
    window.clearWindow();
    createPartyView = CreatePartyView();
    listenToCreatePartyView(createPartyView);
    window.showView(createPartyView);
}

void MenuController::onMainViewSearchPartyButtonClicked() {
    emit nuevoEvento(std::make_shared<ListEvent>());
    // window.clearWindow();
    // searchPartyView = SearchPartyView();
    // listenToSearchPartyView(searchPartyView);
    // window.showView(searchPartyView);
}

void MenuController::onCreatePartyViewCreateButtonClicked() {
    QLineEdit *partyNameTextField = createPartyView.getPartyNameTextField();
    std::string partyName = partyNameTextField->text().toStdString();
    emit nuevoEvento(std::make_shared<CreateEvent>(partyName));
    // window.clearWindow();
    // listenToPartyView(partyView);
    // window.showView(partyView);

}

void MenuController::onCreatePartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}


void MenuController::onConnectViewConnectButtonClicked() {
    QLineEdit *portTextField = connectToServerView.getPortTextField();
    QLineEdit *addressTextField = connectToServerView.getAddressTextField();
    QLineEdit *nameTextField = connectToServerView.getNameTextField();
    std::string port = portTextField->text().toStdString();
    std::string address = addressTextField->text().toStdString();
    std::string name = nameTextField->text().toStdString();
    emit connectRequest(name, address, port);

}

void MenuController::onConnectionRequestResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}

void MenuController::onConnectViewBackButtonClicked() {
    window.clearWindow();
}

void MenuController::onSearchPartyViewJoinButtonClicked(const std::string& partyName) {
    emit nuevoEvento(std::make_shared<JoinEvent>(partyName));

}

void MenuController::onSearchPartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}



void MenuController::onPartyListReceived(const std::vector<std::string>& parties, const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    window.clearWindow();
    searchPartyView = SearchPartyView(parties);
    listenToSearchPartyView(searchPartyView);
    window.showView(searchPartyView);
    
}

void MenuController::onLobbyPlayersReceived(const std::vector<std::string>& players, const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    partyView.clearPlayers();
    for (size_t i = 0; i < players.size(); i++) {
        partyView.addPlayer(players[i]);
    }
}


void MenuController::onJoinPartyResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    window.clearWindow();
    partyView = PartyView(pName);
    listenToPartyView(partyView);
    window.showView(partyView);

}

void MenuController::onCreatePartyResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    QLineEdit *partyNameTextField = createPartyView.getPartyNameTextField();
    std::string partyName = partyNameTextField->text().toStdString();
    window.clearWindow();
    partyView = PartyView(partyName);
    listenToPartyView(partyView);
    window.showView(partyView);
}

void MenuController::onLeavePartyResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        return;
    }
    window.clearWindow();
    mainView = MainView();
    listenToMainView(mainView);
    window.showView(mainView);
}


void MenuController::onLobbyReady() {
    QPushButton *startButton = partyView.getStartButton();
    startButton->setEnabled(true);
}
void MenuController::onLobbyNotReady() {
    QPushButton *startButton = partyView.getStartButton();
    startButton->setEnabled(false);
}