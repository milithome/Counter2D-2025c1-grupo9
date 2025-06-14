#ifndef QTWINDOW_H
#define QTWINDOW_H


#include "qtview.h"
#include <QMainWindow>
#include <QString>

class QtWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QtWindow(const std::string& windowName, int width, int height);
    void showView(QtView& view);
    void clearWindow();
    void quit();
    QPoint getPosition();

    //void show() { window.show(); };

private:
    QWidget window = QWidget(this);
    void deleteLayoutRecursively(QLayout* layout);
    void closeEvent(QCloseEvent* event) override {
        emit windowClosed();
        QWidget::closeEvent(event); // permite que Qt haga su trabajo de cierre
    }

signals:
    void windowClosed();
};

#endif