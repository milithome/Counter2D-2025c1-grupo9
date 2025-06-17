
#ifndef PARTY_VIEW_H
#define PARTY_VIEW_H

#include "game_view.h"
#include "qtview.h"
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QDialog>

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
    std::unordered_map<tSkin, QPushButton*> getTSkinButtons() { return tSkins; };
    std::unordered_map<ctSkin, QPushButton*> getCtSkinButtons() { return ctSkins; };
    QComboBox *getMapCombo() { return mapCombo; };
    QDialog *getSettingsModal() { return settingsModal; };
    QPushButton *getSettingsButton() { return settingsButton; };
    void showModal() {
        settingsModal->exec();
    }

    void addPlayer(const std::string& player);
    void clearPlayers();
private:
    std::string partyName; 
    std::vector<std::string> players;
    QLabel *partyNameLabel = nullptr;
    QPushButton *startButton = nullptr;
    QPushButton *leaveButton = nullptr;
    QVBoxLayout *layout = nullptr;
    QListWidget *playerList = nullptr;
    QDialog *settingsModal = nullptr;
    QPushButton *settingsButton = nullptr;

    QComboBox *mapCombo = nullptr;
    std::unordered_map<tSkin, QPushButton*> tSkins;
    std::unordered_map<ctSkin, QPushButton*> ctSkins;

    void buildSettingsButton();
    void buildPartyNameLabel();
    void buildStartButton();
    void buildLeaveButton();
    void buildPlayerList();
    void buildModal();
    QWidget* createCtSkinsColumn(const std::unordered_map<ctSkin, QString>& imagePaths);
    QWidget* createTSkinsColumn(const std::unordered_map<tSkin, QString>& imagePaths);
    QPushButton* createSkinButton(const QString& imagePath);
    
    void addPlayerToList(const std::string& playerName);
};

#endif