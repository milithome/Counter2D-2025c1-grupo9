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


SearchPartyView::SearchPartyView() {};


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
    std::vector<std::string> parties = {"party uno", "party dos", "party tres"};
    partyList = new QListWidget();

    for (const std::string& partyName : parties) {
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
}

