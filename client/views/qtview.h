#ifndef QTVIEW_H
#define QTVIEW_H

#include <QWidget>

class QtView {
public:
    virtual ~QtView() = 0;
    virtual QLayout *getLayout();
    virtual void buildLayout();
private:
};

#endif