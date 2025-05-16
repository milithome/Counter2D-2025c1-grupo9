#include "create_party_view.h"
#include <QHBoxLayout>

CreatePartyView::CreatePartyView() {
};


QPushButton *CreatePartyView::getCreateButton() {
    return createButton;
};
QPushButton *CreatePartyView::getBackButton() {
    return backButton;
};

QLabel *CreatePartyView::getPartyNameLabel() {
    return partyNameLabel;
};

QLineEdit *CreatePartyView::getPartyNameTextField() {
    return partyNameTextField;
};

void CreatePartyView::buildPartyNameLabel() {
    partyNameLabel = new QLabel("Nombre de la partida");
    partyNameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void CreatePartyView::buildPartyNameTextField() {
    partyNameTextField = new QLineEdit();
}

void CreatePartyView::buildBackButton() {
    backButton = new QPushButton("Atrás");
}

void CreatePartyView::buildCreateButton() {
    createButton = new QPushButton("Crear");
}

void CreatePartyView::buildLayout() {
    buildPartyNameLabel();
    buildPartyNameTextField();
    buildBackButton();
    buildCreateButton();

    layout = new QVBoxLayout();
    
    layout->setContentsMargins(180, 200, 180, 200);

    layout->addWidget(partyNameLabel);
    layout->addWidget(partyNameTextField);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(backButton);
    buttonsLayout->addWidget(createButton);

    layout->addLayout(buttonsLayout);
}

QVBoxLayout *CreatePartyView::getLayout() {
    return layout;
}