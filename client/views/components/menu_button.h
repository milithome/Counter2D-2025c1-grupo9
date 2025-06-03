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

    ~MenuButton() {
        delete effect;
    }
    MenuButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(0, 0, 0, 0);"
            "   color: white;"
            "   border-radius: 10px;" // esta linea de codigo hace q magicamente el boton sea transparente
            "}"
            "QPushButton:hover {"
            "   color: yellow;"                             
            "}"
            "QPushButton:disabled {"
            "   color: rgba(255, 255, 255, 128);" 
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
        if (isEnabled()) {
            effect->setColor(Qt::yellow);
        }



        QPushButton::enterEvent(event);

    }

    void leaveEvent(QEvent *event) override {
        effect->setColor(Qt::white);


        QPushButton::leaveEvent(event);
    }
};

#endif