
#ifndef PARTY_VIEW_H
#define PARTY_VIEW_H

#include "qtview.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

class PartyView : public QtView {
public:
    PartyView();
    PartyView(const std::string& partyName);
    PartyView(const std::string& partyName, const std::vector<std::string>& players);
    void buildLayout() override;
    QVBoxLayout *getLayout() override;
    QLabel *getPartyNameLabel();
    QPushButton *getStartButton();
    QPushButton *getLeaveButton();
    QListWidget *getPlayerList();
    void addPlayer(const std::string& player);
private:
    std::string partyName; 
    std::vector<std::string> players;
    QLabel *partyNameLabel = nullptr;
    QPushButton *startButton = nullptr;
    QPushButton *leaveButton = nullptr;
    QVBoxLayout *layout = nullptr;
    QListWidget *playerList = nullptr;

    void buildPartyNameLabel();
    void buildStartButton();
    void buildLeaveButton();
    void buildPlayerList();
    void addPlayerToList(const std::string& playerName);
};

#endif