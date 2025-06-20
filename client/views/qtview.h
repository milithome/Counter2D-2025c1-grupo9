#ifndef QTVIEW_H
#define QTVIEW_H

#include <QWidget>
#include <QMessageBox>

class QtView : public QWidget {
public:
    virtual ~QtView() {};
    virtual QLayout *getLayout() = 0;
    virtual void buildLayout() = 0;
    void showMessage(const std::string& message) {
        QMessageBox::critical(this, "Error", message.c_str());
    }
private:
};

#endif