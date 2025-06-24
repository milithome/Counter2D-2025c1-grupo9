#ifndef CONNECT_TO_SERVER_VIEW_H
#define CONNECT_TO_SERVER_VIEW_H

#include "qtview.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class ConnectToServerView : public QtView {
public:
    ConnectToServerView();
    QPushButton *getConnectButton();
    QLabel *getPortLabel();
    QLineEdit *getPortTextField();
    QLineEdit *getAddressTextField();
    QLineEdit *getNameTextField();
    QVBoxLayout *getLayout() override;
    void buildLayout() override;

private:
    QLabel *portLabel = nullptr;
    QLineEdit *portTextField = nullptr;
    QLabel *addressLabel = nullptr;
    QLineEdit *addressTextField = nullptr;
    QLabel *nameLabel = nullptr;
    QLineEdit *nameTextField = nullptr;
    QPushButton *connectButton = nullptr;
    QVBoxLayout *layout = nullptr;

    void buildPortLabel();
    void buildPortTextField();

    void buildAddressLabel();
    void buildAddressTextField();
    void buildNameLabel();
    void buildNameTextField();

    void buildBackButton();
    void buildConnectButton();
};

#endif