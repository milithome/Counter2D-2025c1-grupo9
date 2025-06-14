
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
    window.setFixedSize(width, height);
    setWindowTitle(QString::fromStdString(window_name));


    // Centrar ventana
    QScreen *screen = QGuiApplication::primaryScreen();

    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    // window.move(x, y);
    move(x, y);



    window.setObjectName("MainWindow");
    window.setStyleSheet(
        "QWidget#MainWindow {"
        "  background-image: url(:/assets/gfx/cs2d.png);"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}"
    );
    //window.show();
    show();
}

void QtWindow::showView(QtView& view) {
    window.setLayout(view.getLayout());
}

void QtWindow::deleteLayoutRecursively(QLayout* layout) {
    if (!layout) return;

    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->hide();
            delete widget;
            delete item;
        } else if (QLayout* childLayout = item->layout()) {
            deleteLayoutRecursively(childLayout);
        } else {
            delete item;
        }
    }

    delete layout;
}

void QtWindow::clearWindow() {
    QLayout* currentLayout = window.layout();
    deleteLayoutRecursively(currentLayout);
}

void QtWindow::quit() {
    hide();
}


QPoint QtWindow::getPosition() {
    return pos();
}
