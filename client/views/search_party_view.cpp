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
#include "components/menu_button.h"
#include "components/menu_label.h"
#include "components/translucent_container.h"

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

    subLayout->addWidget(backButton, 0, Qt::AlignLeft);
    subLayout->addWidget(createSeparator());
    subLayout->addWidget(partyList);

    TranslucentContainer *container = new TranslucentContainer;
    container->addLayout(subLayout);

    layout->addWidget(container);
}


void SearchPartyView::buildBackButton() {
    backButton = new MenuButton("Atrás");
}

void SearchPartyView::buildPartyList() {
    partyList = new QListWidget();

    partyList->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(0, 0, 0, 0);"
        "   border-radius: 10px;"
        "}"
    );
    partyList->setSelectionMode(QAbstractItemView::NoSelection);

    for (const std::string& partyName : parties) {
        addPartyToList(partyName);
    }
}


void SearchPartyView::addPartyToList(const std::string& partyName) {

    QWidget *itemWidget = new QWidget();
    QVBoxLayout *wrapperLayout = new QVBoxLayout(itemWidget);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(contentWidget);
    itemLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *label = new MenuLabel(QString::fromStdString(partyName), contentWidget);
    QPushButton *joinButton = new MenuButton("Unirse", contentWidget);
    joinButtons[partyName] = joinButton;

    itemLayout->addWidget(label);
    itemLayout->addStretch();
    itemLayout->addWidget(joinButton);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setStyleSheet("color: rgba(255, 255, 255, 30); margin-left: 5px; margin-right: 5px;");
    separator->setFixedHeight(1);

    wrapperLayout->addWidget(contentWidget);
    wrapperLayout->addWidget(separator);

    QListWidgetItem *listItem = new QListWidgetItem(partyList);
    listItem->setFlags(Qt::NoItemFlags);
    listItem->setSizeHint(itemWidget->sizeHint());

    partyList->addItem(listItem);
    partyList->setItemWidget(listItem, itemWidget);
}
