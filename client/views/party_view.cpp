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
}

PartyView::PartyView(const std::string& partyName) : partyName(partyName), players({}) {
    buildLayout();
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

    QHBoxLayout *subsubLayout = new QHBoxLayout();
    subsubLayout->addWidget(partyNameLabel);
    subsubLayout->addWidget(settingsButton);

    subLayout->addLayout(subsubLayout);
    subLayout->addWidget(createSeparator());
    subLayout->addWidget(playerList);
    subLayout->addWidget(createSeparator());
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(leaveButton);
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
    QPixmap pixmap(":/images/skin1.png");
    button->setIcon(QIcon(pixmap));
    button->setIconSize(QSize(64, 64));
    button->setFixedSize(70, 70);
    button->setFlat(true);
    return button;
}

QWidget* PartyView::createCtSkinsColumn(const std::unordered_map<ctSkin, QString>& imagePaths) {
    QVBoxLayout* vbox = new QVBoxLayout;

    QLabel* label = new QLabel("CT Skins");
    vbox->addWidget(label);

    QGridLayout* grid = new QGridLayout;
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

    QLabel* label = new QLabel("T Skins");
    vbox->addWidget(label);

    QGridLayout* grid = new QGridLayout;
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
    std::unordered_map<ctSkin, QString> ctSkinPaths = {
        {
            ctSkin::FRENCH_GIGN, ":/assets/gfx/player/ct1.bmp"
        }, 
        {
            ctSkin::GERMAN_GSG9, ":/assets/gfx/player/ct2.bmp"
        }, 
        {
            ctSkin::SEAL_FORCE, ":/assets/gfx/player/ct3.bmp"
        },
        {
            ctSkin::UKSAS, ":/assets/gfx/player/ct4.bmp"
        }
    };
    std::unordered_map<tSkin, QString> tSkinPaths = {
        {
            tSkin::ARCTIC_AVENGER, ":/assets/gfx/player/t1.bmp"
        }, 
        {
            tSkin::GUERRILLA, ":/assets/gfx/player/t2.bmp"
        }, 
        {
            tSkin::L337_KREW, ":/assets/gfx/player/t3.bmp"
        },
        {
            tSkin::PHOENIX, ":/assets/gfx/player/t4.bmp"
        }
    };
    
    topLayout->addWidget(createTSkinsColumn(tSkinPaths));
    topLayout->addWidget(createCtSkinsColumn(ctSkinPaths));

    mainLayout->addLayout(topLayout);

    // Parte inferior con dificultad
    QVBoxLayout* bottomLayout = new QVBoxLayout;
    bottomLayout->addWidget(new QLabel("Seleccionar mapa"));

    QComboBox* mapBox = new QComboBox;
    mapBox->addItems({"default", "grande"});
    bottomLayout->addWidget(mapBox);

    mainLayout->addLayout(bottomLayout);

    // Botón cerrar (opcional)
    QPushButton* closeBtn = new QPushButton("Cerrar");
    QObject::connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    mainLayout->addWidget(closeBtn);

    dialog->setLayout(mainLayout);
    settingsModal = dialog;
}