#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include "qtview.h"

class MainView : public QtView {
public:
    MainView();
    void buildLayout() override;
    QVBoxLayout *getLayout() override;
    QLabel *getTitleLabel();
    QPushButton *getCreateButton();
    QPushButton *getSearchButton();
private:
    QLabel *titleLabel = nullptr;
    QPushButton *createButton = nullptr;
    QPushButton *searchButton = nullptr;
    QVBoxLayout *layout = nullptr;

    void buildTitleLabel();
    void buildCreateButton();
    void buildSearchButton();

};

#endif