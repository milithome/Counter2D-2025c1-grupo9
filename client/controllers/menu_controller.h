#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <string>
#include <functional>
#include "client/views/qtwindow.h"
#include "client/views/main_view.h"
#include "client/views/create_party_view.h"
#include "client/views/search_party_view.h"

class MenuController {
public:
    // using Callback = std::function<void(const std::string&)>;

    MenuController(QtWindow& window);

    // // Set a callback to be called when a menu option is selected
    // void setOptionSelectedCallback(Callback callback);

    // // Process user input (e.g., key press or mouse click)
    // void processInput(const std::string& input);

    // // Render the menu (implementation depends on your UI framework)
    // void render() const;

private:
    QtWindow& window;
    MainView mainView;
    CreatePartyView createPartyView;
    SearchPartyView searchPartyView;



    void onMainViewCreatePartyButtonClicked();
    void onMainViewSearchPartyButtonClicked();
    void onCreatePartyViewCreateButtonClicked();
    void onCreatePartyViewBackButtonClicked();
    void onSearchPartyViewJoinButtonClicked(const std::string& partyName);
    void onSearchPartyViewBackButtonClicked();

    void listenToMainView();
    void listenToSearchPartyView();
    void listenToCreatePartyView();

};

#endif 