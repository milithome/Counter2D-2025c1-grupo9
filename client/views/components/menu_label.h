#ifndef MENU_LABEL_H
#define MENU_LABEL_H

#include <QFont>
#include <QString>
#include <QFontDatabase>
#include <QEvent>

#include <QPushButton>
#include <QGraphicsDropShadowEffect>

class MenuLabel : public QLabel {
    QGraphicsDropShadowEffect *effect;

public:
    MenuLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
    setStyleSheet(
        "color: white;"
    );
    int fontId = QFontDatabase::addApplicationFont(":/assets/gfx/fonts/sourcesans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont sourceFont(fontFamily);
    sourceFont.setPointSize(11);
    setFont(sourceFont);
    }

};

#endif 
    
    
    