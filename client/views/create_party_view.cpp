#include "create_party_view.h"
#include <QHBoxLayout>
#include <QFontDatabase>
#include "components/menu_button.h"
#include "components/translucent_container.h"
#include "components/menu_label.h"

CreatePartyView::CreatePartyView() {
    buildLayout();
}

QPushButton *CreatePartyView::getCreateButton() {
    return createButton;
}

QPushButton *CreatePartyView::getBackButton() {
    return backButton;
}

QLabel *CreatePartyView::getPartyNameLabel() {
    return partyNameLabel;
}

QLineEdit *CreatePartyView::getPartyNameTextField() {
    return partyNameTextField;
}

void CreatePartyView::buildPartyNameLabel() {
    partyNameLabel = new MenuLabel("Nombre de la partida");
}

void CreatePartyView::buildPartyNameTextField() {
    partyNameTextField = new QLineEdit();


    partyNameTextField->setStyleSheet(
        "QLineEdit {"
        "   border: none;"
        "   border-bottom: 1px solid rgba(255, 255, 255, 60);"
        "   background: transparent;"
        "   color: white;"
        "   padding: 2px 0;"
        "}"
        "QLineEdit:focus {"
        "   border-bottom: 1px solid yellow;"
        "}"
    );

    int fontId = QFontDatabase::addApplicationFont(":/assets/gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(11);
    partyNameTextField->setFont(sourceFont);
}

void CreatePartyView::buildBackButton() {
    backButton = new MenuButton("Atrás");
}

void CreatePartyView::buildCreateButton() {
    createButton = new MenuButton("Crear");
}

void CreatePartyView::buildLayout() {
    buildPartyNameLabel();
    buildPartyNameTextField();
    buildBackButton();
    buildCreateButton();

    layout = new QVBoxLayout();
    
    layout->setContentsMargins(200, 0, 200, 0);
    layout->addStretch();

    QVBoxLayout *subLayout = new QVBoxLayout();
    subLayout->addWidget(partyNameLabel);
    subLayout->addWidget(partyNameTextField);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(backButton);
    buttonsLayout->addWidget(createButton);
    subLayout->addLayout(buttonsLayout);
    
    TranslucentContainer *container = new TranslucentContainer();
    container->addLayout(subLayout);


    layout->addWidget(container);
    layout->addStretch();

    setLayout(layout);
}

QVBoxLayout *CreatePartyView::getLayout() {
    return layout;
}
