#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QFrame>
#include <QDebug>
#include "mapEditorTemplate.h"

class QPushButton;
class QLineEdit;
class QSpinBox;
class QGridLayout;

class MapEditor : public QWidget {
    Q_OBJECT

public:
    MapEditor(QWidget* parent = nullptr);

private slots:
    void createNewMap();
    void selectBlockType(BlockType type);
    void handleCellClick(int x, int y);

private:
    void drawGrid();
    void setupSidebar();

    MapTemplate* currentMap;
    BlockType selectedBlockType;

    QGridLayout* gridLayout;
    QWidget* gridWidget;
    QWidget* sidebarWidget;
};

#endif // MAP_EDITOR_H
