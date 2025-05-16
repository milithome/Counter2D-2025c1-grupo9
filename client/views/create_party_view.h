#ifndef CREATE_PARTY_VIEW_H
#define CREATE_PARTY_VIEW_H

#include "qtview.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class CreatePartyView : public QtView {
public:
    CreatePartyView();
    QPushButton *getCreateButton();
    QPushButton *getBackButton();
    QLabel *getPartyNameLabel();
    QLineEdit *getPartyNameTextField();
    QVBoxLayout *getLayout() override;
    void buildLayout() override;
private:
    QLabel *partyNameLabel = nullptr;
    QLineEdit *partyNameTextField = nullptr;
    QPushButton *backButton = nullptr;
    QPushButton *createButton = nullptr;
    QVBoxLayout *layout = nullptr;

    void buildPartyNameLabel();
    void buildPartyNameTextField();
    void buildBackButton();
    void buildCreateButton();
};

#endif