#ifndef QTWINDOW_H
#define QTWINDOW_H


#include "qtview.h"
#include <QMainWindow>
#include <QString>

class QtWindow
{
public:
    explicit QtWindow(const std::string& windowName, int width, int height);
    void showView(QtView& view);
    void clearWindow();
    void quit();
    QPoint getPosition();

    void show() { window.show(); };

private:
    QWidget window;
    void deleteLayoutRecursively(QLayout* layout);
};

#endif