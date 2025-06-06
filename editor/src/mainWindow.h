#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client/views/qtwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480


class MainWindow : public QMainWindow {

public:
    MainWindow(int argc, char* argv[]);
    void run();
    ~MainWindow();

private:
    QApplication app;
    QtWindow menuWindow;
    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;

    void initSDLInWidget();
};

#endif // MAINWINDOW_H
