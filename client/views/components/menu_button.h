#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include <QFont>
#include <QString>
#include <QFontDatabase>
#include <QEvent>

#include <QPushButton>
#include <QGraphicsDropShadowEffect>

class MenuButton : public QPushButton {
    QGraphicsDropShadowEffect *effect;

public:
    MenuButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(0, 0, 0, 0);"
            "   color: white;"
            "   border-radius: 10px;"
            "   padding: 0px;"
            "}"
            "QPushButton:hover {"
            "   color: yellow;"                             
            "}"
        );
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        setAutoFillBackground(true);
        
        int fontId = QFontDatabase::addApplicationFont(":/assets/gfx/fonts/sourcesans.ttf");
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont sourceFont(fontFamily);
        sourceFont.setPointSize(11);
        setFont(sourceFont);

        effect = new QGraphicsDropShadowEffect(this);
        effect->setBlurRadius(13);
        effect->setColor(Qt::white);
        effect->setOffset(0);
        setGraphicsEffect(effect);
    }

protected:
    void enterEvent(QEnterEvent *event) override {
        effect->setColor(Qt::yellow);
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        effect->setColor(Qt::white);
        QPushButton::leaveEvent(event);
    }
};

#endif