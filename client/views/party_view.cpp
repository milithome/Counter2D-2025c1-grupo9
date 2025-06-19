#include "party_view.h"
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
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPixmap>
#include <QPushButton>



PartyView::PartyView() {}

PartyView::PartyView(const std::string& partyName, const std::vector<std::string>& players) : partyName(partyName), players(players) {
    buildLayout();
    buildModal();
}

PartyView::PartyView(const std::string& partyName) : partyName(partyName), players({}) {
    buildLayout();
    buildModal();
}

void PartyView::addPlayer(const std::string& player) {
    players.push_back(player);
    addPlayerToList(player);
}


void PartyView::buildSettingsButton() {
    settingsButton = new MenuButton("Settings");
}

void PartyView::buildLayout() {
    buildPartyNameLabel();
    buildSettingsButton();
    buildPlayerList();
    buildStartButton();
    buildLeaveButton();


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

    subLayout->addWidget(partyNameLabel);
    subLayout->addWidget(createSeparator());
    subLayout->addWidget(playerList);
    subLayout->addWidget(createSeparator());
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(leaveButton);
    buttonsLayout->addWidget(settingsButton);
    buttonsLayout->addWidget(startButton);
    subLayout->addLayout(buttonsLayout);

    TranslucentContainer* container = new TranslucentContainer;
    container->addLayout(subLayout);
    layout->addWidget(container);
}

QVBoxLayout* PartyView::getLayout() {
    return layout;
}

QLabel* PartyView::getPartyNameLabel() {
    return partyNameLabel;
}

QPushButton* PartyView::getStartButton() {
    return startButton;
}

QPushButton* PartyView::getLeaveButton() {
    return leaveButton;
}

QListWidget* PartyView::getPlayerList() {
    return playerList;
}

void PartyView::buildPartyNameLabel() {
    partyNameLabel = new MenuLabel(QString::fromStdString(partyName));
}

void PartyView::buildStartButton() {
    startButton = new MenuButton("Iniciar partida");
    startButton->setEnabled(false);
}

void PartyView::buildLeaveButton() {
    leaveButton = new MenuButton("Atrás");
}

void PartyView::buildPlayerList() {
    playerList = new QListWidget();
    playerList->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(0, 0, 0, 0);"
        "   border-radius: 10px;"
        "}"
    );
    playerList->setSelectionMode(QAbstractItemView::NoSelection);
    
    for (const std::string& playerName : players) {
        addPlayerToList(playerName);
    }
}

void PartyView::addPlayerToList(const std::string& playerName) {

    QWidget *itemWidget = new QWidget();
    QVBoxLayout *wrapperLayout = new QVBoxLayout(itemWidget);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setStyleSheet("color: rgba(255, 255, 255, 30); margin-left: 5px; margin-right: 5px;");
    separator->setFixedHeight(1);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *itemLayout = new QHBoxLayout(contentWidget);
    itemLayout->setContentsMargins(5, 5, 5, 5);

    QLabel *label = new MenuLabel(QString::fromStdString(playerName), contentWidget);
    itemLayout->addWidget(label);

    wrapperLayout->addWidget(contentWidget);
    wrapperLayout->addWidget(separator);

    QListWidgetItem *listItem = new QListWidgetItem(playerList);
    listItem->setFlags(Qt::NoItemFlags);
    listItem->setSizeHint(itemWidget->sizeHint());

    playerList->addItem(listItem);
    playerList->setItemWidget(listItem, itemWidget);
}

void PartyView::clearPlayers() {
    players.clear();
    playerList->clear();
}






QPushButton* PartyView::createSkinButton(const QString& imagePath) {
    QPushButton* button = new QPushButton;

    QPixmap originalPixmap(imagePath);

    QPixmap croppedPixmap = originalPixmap.copy(0, 0, 32, 32); // Top-left 32x32

    QPixmap scaledPixmap = croppedPixmap.scaled(48, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    button->setIcon(QIcon(scaledPixmap));
    button->setIconSize(QSize(48, 48));
    button->setFixedSize(64, 64);
    button->setFlat(true);

    return button;
}

QWidget* PartyView::createCtSkinsColumn(const std::unordered_map<ctSkin, QString>& imagePaths) {
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setContentsMargins(0,0,0,0);

    QLabel* label = new MenuLabel("CT Skins");
    vbox->addWidget(label);

    QGridLayout* grid = new QGridLayout;
    grid->setContentsMargins(0,0,0,0);
    int row = 0, col = 0;
    for (auto [skin, imagePath] : imagePaths) {

        QPushButton* skinLabel = createSkinButton(imagePath);
        ctSkins[skin] = skinLabel;


        grid->addWidget(skinLabel, row, col);
        if (++col >= 2) { 
            col = 0;
            ++row;
        }
    }

    QWidget* gridWidget = new QWidget;
    gridWidget->setLayout(grid);
    vbox->addWidget(gridWidget);

    QWidget* container = new QWidget;
    container->setLayout(vbox);

    return container;
}

QWidget* PartyView::createTSkinsColumn(const std::unordered_map<tSkin, QString>& imagePaths) {
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setContentsMargins(0,0,0,0);

    QLabel* label = new MenuLabel("T Skins");
    vbox->addWidget(label);

    QGridLayout* grid = new QGridLayout;
    grid->setContentsMargins(0,0,0,0);
    int row = 0, col = 0;
    for (auto [skin, imagePath] : imagePaths) {

        QPushButton* skinLabel = createSkinButton(imagePath);
        tSkins[skin] = skinLabel;

        grid->addWidget(skinLabel, row, col);
        if (++col >= 2) { 
            col = 0;
            ++row;
        }
    }

    QWidget* gridWidget = new QWidget;
    gridWidget->setLayout(grid);
    vbox->addWidget(gridWidget);

    QWidget* container = new QWidget;
    container->setLayout(vbox);

    return container;
}


void PartyView::buildModal() {
    QDialog* dialog = new QDialog();
    dialog->setWindowTitle("Settings");

    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Parte superior con dos columnas
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setSpacing(32);
    std::unordered_map<ctSkin, QString> ctSkinPaths = {
        {
            ctSkin::SEAL_FORCE, ":/assets/gfx/player/ct1.bmp"
        }, 
        {
            ctSkin::GERMAN_GSG9, ":/assets/gfx/player/ct2.bmp"
        }, 
        {
            ctSkin::UKSAS, ":/assets/gfx/player/ct3.bmp"
        },
        {
            ctSkin::FRENCH_GIGN, ":/assets/gfx/player/ct4.bmp"
        }
    };
    std::unordered_map<tSkin, QString> tSkinPaths = {
        {
            tSkin::PHOENIX, ":/assets/gfx/player/t1.bmp"
        }, 
        {
            tSkin::L337_KREW, ":/assets/gfx/player/t2.bmp"
        }, 
        {
            tSkin::ARCTIC_AVENGER, ":/assets/gfx/player/t3.bmp"
        },
        {
            tSkin::GUERRILLA, ":/assets/gfx/player/t4.bmp"
        }
    };
    
    topLayout->addWidget(createTSkinsColumn(tSkinPaths));
    topLayout->addWidget(createCtSkinsColumn(ctSkinPaths));

    mainLayout->addLayout(topLayout);

    QVBoxLayout* bottomLayout = new QVBoxLayout;
    bottomLayout->addWidget(new MenuLabel("Seleccionar mapa"));

    QComboBox* mapBox = new QComboBox;
    mapBox->addItems({"default", "grande"});
    bottomLayout->addWidget(mapBox);

    mainLayout->addLayout(bottomLayout);

    QPushButton* closeBtn = new MenuButton("Cerrar");
    QObject::connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    mainLayout->addWidget(closeBtn);

    dialog->setLayout(mainLayout);
    settingsModal = dialog;
}