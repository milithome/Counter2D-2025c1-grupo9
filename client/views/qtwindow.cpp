
#include "qtwindow.h"
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QRect>

QtWindow::QtWindow(QApplication& app, const std::string& window_name, int width, int height) : app(app) {
    window.setFixedSize(width, height);
    window.setWindowTitle(QString::fromStdString(window_name));


    // Centrar ventana
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - window.width()) / 2;
    int y = (screenGeometry.height() - window.height()) / 2;
    window.move(x, y);

    window.show();


    // TODO: ponerle un fondo

    // QString path = "/home/manuel/taller/Counter2D-2025c1-grupo9/assets/gfx/cs2d.png";
    // window.setStyleSheet(QString(
    //     "background-image: url(%1);"
    //     "background-repeat: no-repeat;"
    //     "background-position: center;").arg(path));

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
    app.quit();
}


QPoint QtWindow::getPosition() {
    return window.pos();
}