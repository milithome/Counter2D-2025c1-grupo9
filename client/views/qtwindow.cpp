
#include "qtwindow.h"
#include <QLayout>
#include <QLayoutItem>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QRect>
#include <iostream>



QtWindow::QtWindow(const std::string& window_name, int width, int height) {
    resize(width, height);
    setWindowTitle(QString::fromStdString(window_name));

    // Centrar ventana
    QScreen *screen = QGuiApplication::primaryScreen();

    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    move(x, y);

    background->setObjectName("bg");
    background->setStyleSheet(
        "QWidget#bg {"
        "  border-image: url(/var/taller/gfx/cs2d.png) 0 0 0 0 stretch stretch;"
        "}"
    );

    stackedLayout->setContentsMargins(0, 0, 0, 0);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);

    stackedLayout->addWidget(background);
    stackedLayout->addWidget(muteButton);


    QSize hint = muteButton->sizeHint();
    QSize extraSize(hint.width() + 10, hint.height() + 4);

    muteButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    muteButton->setMinimumSize(extraSize);
    muteButton->setMaximumSize(extraSize);

    muteButton->raise();

    setLayout(stackedLayout);

    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("/var/taller/sfx/menu.wav"));

    audioOutput->setVolume(0.35);
    player->setLoops(QMediaPlayer::Infinite);
    player->play();

    QObject::connect(muteButton, &QPushButton::clicked, this, [this, audioOutput]() mutable {
        bool isMuted = audioOutput->isMuted();
        audioOutput->setMuted(!isMuted);
        muteButton->setText(isMuted ? "Mute" : "Unmute");
        QSize hint = muteButton->sizeHint();
        QSize extraSize(hint.width() + 10, hint.height() + 4);
        muteButton->setMinimumSize(extraSize);
        muteButton->setMaximumSize(extraSize);
    });

    show();
}

void QtWindow::showView(QtView *view) {

    currentView = view;
    stackedLayout->addWidget(view);
    stackedLayout->setCurrentWidget(view);
    view->setParent(this);
    muteButton->raise();
    view->show();
    show();

}


void QtWindow::clearWindow() {
    if (currentView) {
        stackedLayout->removeWidget(currentView);
        delete currentView;
    }
}

void QtWindow::quit() {
    hide();
}

QPoint QtWindow::getPosition() {
    return pos();
}
