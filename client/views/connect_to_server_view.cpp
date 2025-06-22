#include "connect_to_server_view.h"
#include <QHBoxLayout>
#include <QFontDatabase>
#include "components/menu_button.h"
#include "components/translucent_container.h"
#include "components/menu_label.h"

ConnectToServerView::ConnectToServerView() {
    buildLayout();
}

QPushButton *ConnectToServerView::getConnectButton() {
    return connectButton;
}

QPushButton *ConnectToServerView::getBackButton() {
    return backButton;
}

QLabel *ConnectToServerView::getPortLabel() {
    return portLabel;
}

QLineEdit *ConnectToServerView::getPortTextField() {
    return portTextField;
}

QLineEdit *ConnectToServerView::getAddressTextField() {
    return addressTextField;
}

QLineEdit *ConnectToServerView::getNameTextField() {
    return nameTextField;
}

void ConnectToServerView::buildPortLabel() {
    portLabel = new MenuLabel("Puerto");
}

void ConnectToServerView::buildPortTextField() {
    portTextField = new QLineEdit();


    portTextField->setStyleSheet(
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

    int fontId = QFontDatabase::addApplicationFont(BASE_PATH + "gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(11);
    portTextField->setFont(sourceFont);
}

void ConnectToServerView::buildAddressLabel() {
    addressLabel = new MenuLabel("Direccion");
}

void ConnectToServerView::buildAddressTextField() {
    addressTextField = new QLineEdit();


    addressTextField->setStyleSheet(
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

    int fontId = QFontDatabase::addApplicationFont(BASE_PATH + "gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(11);
    addressTextField->setFont(sourceFont);
}
void ConnectToServerView::buildNameLabel() {
    nameLabel = new MenuLabel("Nombre");
}


void ConnectToServerView::buildNameTextField() {
    nameTextField = new QLineEdit();


    nameTextField->setStyleSheet(
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

    int fontId = QFontDatabase::addApplicationFont(BASE_PATH + "gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(11);
    nameTextField->setFont(sourceFont);
}


void ConnectToServerView::buildBackButton() {
    backButton = new MenuButton("Atrás");
}

void ConnectToServerView::buildConnectButton() {
    connectButton = new MenuButton("Conectar");
}

void ConnectToServerView::buildLayout() {
    buildPortLabel();
    buildPortTextField();
    buildAddressLabel();
    buildAddressTextField();
    buildNameLabel();
    buildNameTextField();
    buildBackButton();
    buildConnectButton();

    layout = new QVBoxLayout();
    
    layout->setContentsMargins(200, 0, 200, 0);
    layout->addStretch();

    QVBoxLayout *subLayout = new QVBoxLayout();
    subLayout->addWidget(nameLabel);
    subLayout->addWidget(nameTextField);
    subLayout->addWidget(addressLabel);
    subLayout->addWidget(addressTextField);
    subLayout->addWidget(portLabel);
    subLayout->addWidget(portTextField);


    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(backButton);
    buttonsLayout->addWidget(connectButton);
    subLayout->addLayout(buttonsLayout);
    
    TranslucentContainer *container = new TranslucentContainer();
    container->addLayout(subLayout);


    layout->addWidget(container);
    layout->addStretch();

    setLayout(layout);
}

QVBoxLayout *ConnectToServerView::getLayout() {
    return layout;
}
