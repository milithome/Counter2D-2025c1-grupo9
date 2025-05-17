#include "main_view.h"
#include <QTimer>
#include <QDir>

MainView::MainView() {
    buildLayout();
}

void MainView::buildLayout() {
    buildCreateButton();
    buildTitleLabel();
    buildSearchButton();

    layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(createButton);
    layout->addWidget(searchButton);
    layout->addStretch();
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
    QFont titleFont;

    titleLabel->setStyleSheet(
        "color: white;"
        "background-color: rgba(0, 0, 0, 128);" 
        "font-weight: bold;"
        "font-size: 24px;"
    );
}

void MainView::buildCreateButton() {
    createButton = new QPushButton("Crear partida");
    createButton->setStyleSheet(
        "background-color: rgba(0, 0, 0, 128);"
        "color: white;"
        "border-radius: 10px;"
        "padding: 10px;"
    );
    createButton->setAutoFillBackground(true);
}

void MainView::buildSearchButton() {
    searchButton = new QPushButton("Buscar partida");
    searchButton->setStyleSheet(
        "background-color: rgba(0, 0, 0, 128);"
        "color: white;"
        "border-radius: 10px;"
        "padding: 10px;"
    );
    searchButton->setAutoFillBackground(true);
}