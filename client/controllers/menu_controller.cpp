#include "menu_controller.h"
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include "messages/join_event.h"
#include "messages/leave_event.h"
#include "messages/list_event.h"
#include "messages/create_event.h"
#include "messages/start_event.h"
#include "messages/t_skin_picked_event.h"
#include "messages/ct_skin_picked_event.h"
#include "messages/map_picked_event.h"

MenuController::MenuController(QtWindow& window) : QWidget(nullptr), window(window) {
    connectToServerView = ConnectToServerView();
    listenToConnectView();
    window.showView(connectToServerView);
}

void MenuController::listenToMainView() {
    QPushButton *searchButton = mainView.getSearchButton();
    QPushButton *createButton = mainView.getCreateButton(); 

    QObject::connect(createButton, &QPushButton::clicked, [this]() {
        onMainViewCreatePartyButtonClicked();
    });
    QObject::connect(searchButton, &QPushButton::clicked, [this]() {
        onMainViewSearchPartyButtonClicked();
    });
}

void MenuController::listenToCreatePartyView() {
    QPushButton *backButton = createPartyView.getBackButton();
    QPushButton *createButton = createPartyView.getCreateButton();
    QObject::connect(createButton, &QPushButton::clicked, [this]() {
        onCreatePartyViewCreateButtonClicked();
    });
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onCreatePartyViewBackButtonClicked();
    });
}

void MenuController::listenToConnectView() {
    QPushButton *backButton = connectToServerView.getBackButton();
    QPushButton *connectButton = connectToServerView.getConnectButton();
    QObject::connect(connectButton, &QPushButton::clicked, [this]() {
        onConnectViewConnectButtonClicked();
    });
    QObject::connect(backButton, &QPushButton::clicked, [this]() {
        onConnectViewBackButtonClicked();
    });
}


void MenuController::listenToSearchPartyView() {
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

void MenuController::listenToPartyView() {
    QPushButton *leaveButton = partyView.getLeaveButton();
    QPushButton *startButton = partyView.getStartButton();
    QPushButton *settingsButton = partyView.getSettingsButton();
    QObject::connect(leaveButton, &QPushButton::clicked, [this]() {
        onPartyViewLeaveButtonClicked();
    });
    QObject::connect(startButton, &QPushButton::clicked, [this]() {
        onPartyViewStartButtonClicked();
    });
    QObject::connect(settingsButton, &QPushButton::clicked, [this]() {
        partyView.showModal();
    });
    std::unordered_map<tSkin, QPushButton*> tSkins = partyView.getTSkinButtons();
    std::unordered_map<ctSkin, QPushButton*> ctSkins = partyView.getCtSkinButtons();

    for (auto [skin, button] : tSkins) {
        QObject::connect(button, &QPushButton::clicked, [this, skin]() {
            emit newMessage(std::make_shared<TSkinPickedEvent>(skin));
        });
    }
    for (auto [skin, button] : ctSkins) {
        QObject::connect(button, &QPushButton::clicked, [this, skin]() {
            emit newMessage(std::make_shared<CtSkinPickedEvent>(skin));
        });
    }
    QComboBox *mapBox = partyView.getMapCombo();
    QObject::connect(mapBox, &QComboBox::currentTextChanged, [this](const QString& text) {
        emit newMessage(std::make_shared<MapPickedEvent>(text.toStdString()));
    });
}

void MenuController::onPartyViewLeaveButtonClicked() {
    emit newMessage(std::make_shared<LeaveEvent>());
}

void MenuController::onPartyViewStartButtonClicked() {
    emit newMessage(std::make_shared<StartEvent>());
}

void MenuController::onGameStarted() {
    window.clearWindow();
}

void MenuController::onMainViewCreatePartyButtonClicked() {
    window.clearWindow();
    createPartyView = CreatePartyView();
    listenToCreatePartyView();
    window.showView(createPartyView);
}

void MenuController::onMainViewSearchPartyButtonClicked() {
    emit newMessage(std::make_shared<ListEvent>());
}

void MenuController::onCreatePartyViewCreateButtonClicked() {
    QLineEdit *partyNameTextField = createPartyView.getPartyNameTextField();
    std::string partyName = partyNameTextField->text().toStdString();
    emit newMessage(std::make_shared<CreateEvent>(partyName));
}

void MenuController::onCreatePartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView();
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

void MenuController::onConnectViewBackButtonClicked() {
    window.clearWindow();
}

void MenuController::onSearchPartyViewJoinButtonClicked(const std::string& partyName) {
    emit newMessage(std::make_shared<JoinEvent>(partyName));

}

void MenuController::onSearchPartyViewBackButtonClicked() {
    window.clearWindow();
    mainView = MainView();
    listenToMainView();
    window.showView(mainView);
}

void MenuController::onConnectionRequestResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        QMessageBox::critical(nullptr, "Error", message.c_str());
        return;
    }
    window.clearWindow();
    mainView = MainView();
    listenToMainView();
    window.showView(mainView);
}


void MenuController::onPartyListReceived(const std::vector<std::string>& parties, const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        QMessageBox::critical(nullptr, "Error", message.c_str());
        return;
    }
    window.clearWindow();
    searchPartyView = SearchPartyView(parties);
    listenToSearchPartyView();
    window.showView(searchPartyView);
    
}

void MenuController::onLobbyPlayersReceived(const std::vector<std::string>& players, const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        QMessageBox::critical(nullptr, "Error", message.c_str());
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
        QMessageBox::critical(nullptr, "Error", message.c_str());
        return;
    }
    window.clearWindow();
    partyView = PartyView(pName);
    listenToPartyView();
    window.showView(partyView);

}

void MenuController::onCreatePartyResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        QMessageBox::critical(nullptr, "Error", message.c_str());
        return;
    }
    QLineEdit *partyNameTextField = createPartyView.getPartyNameTextField();
    std::string partyName = partyNameTextField->text().toStdString();
    window.clearWindow();
    partyView = PartyView(partyName);
    listenToPartyView();
    window.showView(partyView);
}

void MenuController::onLeavePartyResponseReceived(const std::string& message, const uint8_t result) {
    if (result) { // caso error
        std::cout << message << std::endl;
        QMessageBox::critical(nullptr, "Error", message.c_str());
        return;
    }
    window.clearWindow();
    mainView = MainView();
    listenToMainView();
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