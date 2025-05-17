
#include "qtwindow.h"
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>

QtWindow::QtWindow(QApplication& app, const std::string& window_name, int width, int height) : app(app) {
    window.setFixedSize(width, height);
    window.setWindowTitle(QString::fromStdString(window_name));
    window.show();

    // QString path = "/home/manuel/taller/Counter2D-2025c1-grupo9/assets/gfx/cs2d.png";
    // window.setStyleSheet(QString(
    //     "background-image: url(%1);"
    //     "background-repeat: no-repeat;"
    //     "background-position: center;").arg(path));

}

void QtWindow::showView(QtView& view) {
    window.setLayout(view.getLayout());
}

// void QtWindow::clearWindow() {
//     if (window.layout() == nullptr) return;
//     QLayout *currentLayout = window.layout();
//     while (QLayoutItem* item = currentLayout->takeAt(0)) {
//         if (QWidget* widget = item->widget()) {
//             widget->hide();
//             currentLayout->removeWidget(widget);
//             delete widget;
//         }
//         delete item;
//     }
//     delete currentLayout;
//     window.setLayout(nullptr);
// }

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
    // QLayout* currentLayout = window.layout();
    // deleteLayoutRecursively(currentLayout);
}