
#include "qtwindow.h"
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QRect>
#include <iostream>

QtWindow::QtWindow(const std::string& window_name, int width, int height) {
    setFixedSize(width, height);
    setWindowTitle(QString::fromStdString(window_name));


    // Centrar ventana
    QScreen *screen = QGuiApplication::primaryScreen();

    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    move(x, y);

    background->setObjectName("bg");
    background->setStyleSheet(
        "QWidget#bg {"
        "  background-image: url(:/assets/gfx/cs2d.png);"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}"
    );

    stackedLayout->setContentsMargins(0, 0, 0, 0);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);

    stackedLayout->addWidget(background);
    setLayout(stackedLayout);

    show();
}

void QtWindow::showView(QtView *view) {
    currentView = view;
    stackedLayout->addWidget(view);
    stackedLayout->setCurrentWidget(view);
    view->show();
    show();
}


void QtWindow::clearWindow() {
    if (currentView) {
        stackedLayout->removeWidget(currentView);
        delete currentView;
    }
}

void QtWindow::quit() {
    hide();
}

QPoint QtWindow::getPosition() {
    return pos();
}
