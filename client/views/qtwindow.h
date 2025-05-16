#ifndef QTWINDOW_H
#define QTWINDOW_H


#include "qtview.h"
#include <QMainWindow>
#include <QString>

class QtWindow
{
public:
    explicit QtWindow(QApplication& app, const std::string& windowName, int width, int height);
    void showView(QtView& view);
    void clearWindow();


private:
    QApplication& app;
    QWidget window;
    void deleteLayoutRecursively(QLayout* layout);
};

#endif