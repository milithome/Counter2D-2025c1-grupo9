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
#include "components/menu_button.h"
#include "components/menu_label.h"
#include "components/translucent_container.h"



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
    buildPartyNameLabel();
    buildPlayerList();
    buildStartButton();
    buildLeaveButton();

    auto createSeparator = []() {
        QFrame *separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Plain);
        separator->setStyleSheet("color: rgba(255, 255, 255, 30);");
        separator->setFixedHeight(1);
        return separator;
    };


    layout = new QVBoxLayout();

    layout->setContentsMargins(50, 50, 50, 50);

    
    QVBoxLayout *subLayout = new QVBoxLayout();
    
    subLayout->addWidget(partyNameLabel);
    subLayout->addWidget(createSeparator());
    subLayout->addWidget(playerList);
    subLayout->addWidget(createSeparator());
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(leaveButton);
    buttonsLayout->addWidget(startButton);
    subLayout->addLayout(buttonsLayout);

    TranslucentContainer* container = new TranslucentContainer;
    container->addLayout(subLayout);
    layout->addWidget(container);
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
    partyNameLabel = new MenuLabel(QString::fromStdString(partyName));
}

void PartyView::buildStartButton() {
    startButton = new MenuButton("Iniciar partida");
    startButton->setEnabled(false);
}

void PartyView::buildLeaveButton() {
    leaveButton = new MenuButton("Atrás");
}

void PartyView::buildPlayerList() {
    playerList = new QListWidget();
    playerList->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(0, 0, 0, 0);"
        "   border-radius: 10px;"
        "}"
    );
    playerList->setSelectionMode(QAbstractItemView::NoSelection);
    
    for (const std::string& playerName : players) {
        addPlayerToList(playerName);
    }
}

void PartyView::addPlayerToList(const std::string& playerName) {

    QWidget *itemWidget = new QWidget();
    QVBoxLayout *wrapperLayout = new QVBoxLayout(itemWidget);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setStyleSheet("color: rgba(255, 255, 255, 30); margin-left: 5px; margin-right: 5px;");
    separator->setFixedHeight(1);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(contentWidget);
    itemLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *label = new MenuLabel(QString::fromStdString(playerName), contentWidget);
    itemLayout->addWidget(label);

    wrapperLayout->addWidget(contentWidget);
    wrapperLayout->addWidget(separator);

    QListWidgetItem *listItem = new QListWidgetItem(playerList);
    listItem->setFlags(Qt::NoItemFlags);
    listItem->setSizeHint(itemWidget->sizeHint());

    playerList->addItem(listItem);
    playerList->setItemWidget(listItem, itemWidget);
}

void PartyView::clearPlayers() {
    players.clear();
    playerList->clear();
}