#include "party_view.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QScrollArea>
#include <QScrollBar>
#include <QList>
#include <QFrame>



PartyView::PartyView() {}

PartyView::PartyView(const std::string& partyName, const std::vector<std::string>& players) : partyName(partyName), players(players) {
    buildLayout();
}

PartyView::PartyView(const std::string& partyName) : partyName(partyName), players({}) {
    buildLayout();
}

void PartyView::addPlayer(const std::string& player) {
    players.push_back(player);
    addPlayerToList(player);
}


void PartyView::buildLayout() {
    layout = new QVBoxLayout();

    buildPartyNameLabel();
    buildPlayerList();
    buildStartButton();
    buildLeaveButton();

    layout->addWidget(partyNameLabel);
    layout->addWidget(playerList);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(leaveButton);
    buttonsLayout->addWidget(startButton);
    layout->addLayout(buttonsLayout);
}

QVBoxLayout* PartyView::getLayout() {
    return layout;
}

QLabel* PartyView::getPartyNameLabel() {
    return partyNameLabel;
}

QPushButton* PartyView::getStartButton() {
    return startButton;
}

QPushButton* PartyView::getLeaveButton() {
    return leaveButton;
}

QListWidget* PartyView::getPlayerList() {
    return playerList;
}

void PartyView::buildPartyNameLabel() {
    partyNameLabel = new QLabel(QString::fromStdString(partyName));
}

void PartyView::buildStartButton() {
    startButton = new QPushButton("Start");
}

void PartyView::buildLeaveButton() {
    leaveButton = new QPushButton("Back");
}

void PartyView::buildPlayerList() {
    playerList = new QListWidget();

    
    for (const std::string& playerName : players) {
        addPlayerToList(playerName);
    }
}

void PartyView::addPlayerToList(const std::string& playerName) {
    QWidget *itemWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(5, 2, 5, 2);

    QLabel *label = new QLabel(QString::fromStdString(playerName), itemWidget);

    itemLayout->addWidget(label);

    QListWidgetItem *listItem = new QListWidgetItem(playerList);
    listItem->setSizeHint(itemWidget->sizeHint());

    playerList->addItem(listItem);
    playerList->setItemWidget(listItem, itemWidget);
}

void PartyView::clearPlayers() {
    players.clear();
    playerList->clear();
}