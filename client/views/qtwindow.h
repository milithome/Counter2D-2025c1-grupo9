#ifndef QTWINDOW_H
#define QTWINDOW_H


#include "qtview.h"
#include <QMainWindow>
#include <QString>
#include <QStackedLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "components/menu_button.h"

class QtWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QtWindow(const std::string& windowName, int width, int height);

    void showView(QtView *view);
    void clearWindow();
    void quit();
    QPoint getPosition();
    void showMessage(const std::string& message) {
        currentView->showMessage(message);
    }

private:
    QStackedLayout* stackedLayout = new QStackedLayout(this);
    QWidget *background = new QWidget(this);
    QtView *currentView = nullptr;

    QPushButton *muteButton = new MenuButton("Mute", this);
    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    bool muted = false;

    void closeEvent(QCloseEvent* event) override {
        emit windowClosed();
        QWidget::closeEvent(event);
    }

signals:
    void windowClosed();
};

#endif