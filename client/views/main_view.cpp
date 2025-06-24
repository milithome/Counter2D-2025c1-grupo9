#include "main_view.h"
#include <QTimer>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QFontDatabase>
#include "components/menu_button.h"
#include "components/translucent_container.h"

MainView::MainView() {
    buildLayout();
}

void MainView::buildLayout() {
    buildCreateButton();
    buildTitleLabel();
    buildSearchButton();

    layout = new QVBoxLayout();
    layout->setContentsMargins(100, 0, 100, 0);


    layout->addStretch();
    layout->addWidget(titleLabel);

    QVBoxLayout *subLayout = new QVBoxLayout();
    subLayout->addWidget(createButton, 0, Qt::AlignHCenter);
    subLayout->addSpacing(20);
    subLayout->addWidget(searchButton, 0, Qt::AlignHCenter);

    TranslucentContainer* container = new TranslucentContainer;
    container->addLayout(subLayout);

    layout->addSpacing(50);
    layout->addWidget(container, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
}

QVBoxLayout *MainView::getLayout() {
    return layout;
}

QLabel *MainView::getTitleLabel() {
    return titleLabel;
}

QPushButton *MainView::getCreateButton() {
    return createButton;
}

QPushButton *MainView::getSearchButton() {
    return searchButton;
}

void MainView::buildTitleLabel() {
    titleLabel = new QLabel("Counter Strike 2D");
    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(
        "color: yellow;"
        "background-color: rgba(0, 0, 0, 0);" 
    );
    auto *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 0);
    shadow->setColor(Qt::yellow);
    titleLabel->setGraphicsEffect(shadow);

    int fontId = QFontDatabase::addApplicationFont(BASE_PATH + "gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(32);
    sourceFont.setBold(true);
    titleLabel->setFont(sourceFont);
}

void MainView::buildCreateButton() {
    createButton = new MenuButton("Crear partida");
}

void MainView::buildSearchButton() {
    searchButton = new MenuButton("Buscar partida");
}