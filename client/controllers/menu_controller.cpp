#include "menu_controller.h"
#include <QLayout>
#include <QPushButton>
#include <iostream>

MenuController::MenuController(QtWindow& window) : window(window) {
    mainView.buildLayout();
    listenToMainView();
    window.showView(mainView);
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

void MenuController::listenToSearchPartyView() {
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

void MenuController::onMainViewCreatePartyButtonClicked() {
    window.clearWindow();
    createPartyView.buildLayout();
    listenToCreatePartyView();
    window.showView(createPartyView);
}

void MenuController::onMainViewSearchPartyButtonClicked() {
    window.clearWindow();
    searchPartyView.buildLayout();
    listenToSearchPartyView();
    window.showView(searchPartyView);
}

void MenuController::onCreatePartyViewCreateButtonClicked() {
    std::string partyName = createPartyView.getPartyNameTextField()->text().toStdString();
    std::cout << "se creo la party " << partyName << std::endl;
}

void MenuController::onCreatePartyViewBackButtonClicked() {
    window.clearWindow();
    mainView.buildLayout();
    listenToMainView();
    window.showView(mainView);
}

void MenuController::onSearchPartyViewJoinButtonClicked(const std::string& partyName) {
    std::cout << "se unio a " << partyName << std::endl;
}

void MenuController::onSearchPartyViewBackButtonClicked() {
    window.clearWindow();
    mainView.buildLayout();
    listenToMainView();
    window.showView(mainView);
}