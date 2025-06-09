#ifndef EDITORMENUWINDOW_H
#define EDITORMENUWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPixmap>

#include "mapEditor.h"


class QPushButton;

class EditorMenuWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorMenuWindow(QWidget *parent = nullptr);

private:
    QPushButton *nuevoMapaBtn;
    QPushButton *mapasGuardadosBtn;
    QPushButton *volverMenuBtn;

    //void paintEvent(QPaintEvent *event) override; // Override paintEvent to draw the background
};

#endif // EDITORMENUWINDOW_H
