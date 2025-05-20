#include "search_party_view.h"
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


SearchPartyView::SearchPartyView(std::vector<std::string> parties) : parties(parties) {
    buildLayout();
}

SearchPartyView::SearchPartyView() : parties({}) {
    buildLayout();
}

void SearchPartyView::addParty(const std::string& party) {
    parties.push_back(party);
    addPartyToList(party);
}

std::unordered_map<std::string, QPushButton *> SearchPartyView::getJoinButtons() {
    return joinButtons;
}

QPushButton *SearchPartyView::getBackButton(){
    return backButton;
}

QVBoxLayout *SearchPartyView::getLayout() {
    return layout;
}

void SearchPartyView::buildLayout() {
    buildBackButton();
    buildPartyList();

    layout = new QVBoxLayout();
    
    layout->setContentsMargins(50, 50, 50, 50);

    layout->addWidget(backButton);
    layout->addWidget(partyList);
}


void SearchPartyView::buildBackButton() {
    backButton = new QPushButton("Atrás");
}

void SearchPartyView::buildPartyList() {
    partyList = new QListWidget();

    for (const std::string& partyName : parties) {
        addPartyToList(partyName);
    }
}


void SearchPartyView::addPartyToList(const std::string& partyName) {
    QWidget *itemWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(5, 2, 5, 2);

    QLabel *label = new QLabel(QString::fromStdString(partyName), itemWidget);
    QPushButton *joinButton = new QPushButton("Unirse", itemWidget);
    joinButtons[partyName] = joinButton;

    itemLayout->addWidget(label);
    itemLayout->addStretch();
    itemLayout->addWidget(joinButton);

    QListWidgetItem *listItem = new QListWidgetItem(partyList);
    listItem->setSizeHint(itemWidget->sizeHint());

    partyList->addItem(listItem);
    partyList->setItemWidget(listItem, itemWidget);
}
