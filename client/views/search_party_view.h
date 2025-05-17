#ifndef SEARCH_PARTY_VIEW_H
#define SEARCH_PARTY_VIEW_H

#include "qtview.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>

class SearchPartyView : public QtView {
public:
    SearchPartyView(std::vector<std::string> parties);
    std::unordered_map<std::string, QPushButton *> getJoinButtons();
    QPushButton *getBackButton();
    void buildLayout() override;
    QVBoxLayout *getLayout() override;
    void addParty(const std::string& party);
private:
    std::vector<std::string> parties;
    QVBoxLayout *layout = nullptr;
    QListWidget *partyList;
    QPushButton *backButton = nullptr;
    std::unordered_map<std::string, QPushButton *> joinButtons;
    void buildBackButton();
    void buildPartyList();
    void addPartyToList(const std::string& partyName);
};

#endif