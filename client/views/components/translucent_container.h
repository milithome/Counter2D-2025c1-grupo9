#ifndef TRANSLUCENT_CONTAINER_H
#define TRANSLUCENT_CONTAINER_H

#include <QFont>
#include <QString>
#include <QFontDatabase>
#include <QEvent>

#include <QPushButton>
#include <QGraphicsDropShadowEffect>

class TranslucentContainer : public QWidget {
    QVBoxLayout *marginLayout; // define margenes
public:
    TranslucentContainer() {
        setObjectName("backgroundContainer");
        setStyleSheet(
            "#backgroundContainer {"
            "   background-color: rgba(0, 0, 0, 96);"
            "   border-radius: 10px;"
            "}"
        );
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        marginLayout = new QVBoxLayout(this);
    }

    void setContentsMargins(int left, int top, int right, int bottom) {
        marginLayout->setContentsMargins(left, top, right, bottom);
    }

    void addWidget(QWidget *widget) {
        marginLayout->addWidget(widget);
    }

    void addLayout(QLayout *layout) {
        marginLayout->addLayout(layout);
    }

};

#endif