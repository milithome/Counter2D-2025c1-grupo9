
#include "qtwindow.h"
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QRect>
#include <iostream>

QtWindow::QtWindow(const std::string& window_name, int width, int height) {
    resize(width, height);
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
        "  border-image: url(:/assets/gfx/cs2d.png) 0 0 0 0 stretch stretch;"
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



    qDebug() << "== QtWindow State ==";
    qDebug() << "Window size:" << size();
    qDebug() << "Minimum size:" << minimumSize();
    qDebug() << "Size hint:" << sizeHint();

    if (currentView) {
        qDebug() << "Current view:" << currentView;
        qDebug() << "View minimumSize:" << currentView->minimumSize();
        qDebug() << "View sizePolicy:"
                << currentView->sizePolicy().horizontalPolicy()
                << currentView->sizePolicy().verticalPolicy();
        qDebug() << "View sizeHint:" << currentView->sizeHint();
    }
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
