#ifndef QTWINDOW_H
#define QTWINDOW_H


#include "qtview.h"
#include <QMainWindow>
#include <QString>
#include <QStackedLayout>
#include <QMediaPlayer>
#include <QAudioOutput>

class QtWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QtWindow(const std::string& windowName, int width, int height);
    void showView(QtView *view);
    void clearWindow();
    void quit();
    QPoint getPosition();

private:
    QStackedLayout* stackedLayout = new QStackedLayout(this);
    QWidget *background = new QWidget(this);
    QWidget *currentView = nullptr;

    // musica del menu
    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);

    void closeEvent(QCloseEvent* event) override {
        emit windowClosed();
        QWidget::closeEvent(event);
    }

signals:
    void windowClosed();
};

#endif