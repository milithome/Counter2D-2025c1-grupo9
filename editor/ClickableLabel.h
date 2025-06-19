// ClickableLabel.h
#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr)
        : QLabel(parent), seleccionado(false) {}

    void setSeleccionado(bool sel) {
        seleccionado = sel;
        actualizarEstilo();
    }

    bool estaSeleccionado() const {
        return seleccionado;
    }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }

private:
    bool seleccionado;

    void actualizarEstilo() {
        if (seleccionado) {
            this->setStyleSheet("border: 2px solid yellow;");
        } else {
            this->setStyleSheet("border: none;");
        }
    }
};

#endif // CLICKABLELABEL_H
