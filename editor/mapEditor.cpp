#include "mapEditor.h"

class GridCell : public QLabel {
    Q_OBJECT

public:
    GridCell(int x, int y, QWidget* parent = nullptr)
        : QLabel(parent), x(x), y(y) {
        setFixedSize(32, 32);
        setFrameStyle(QFrame::Box);
        setAlignment(Qt::AlignCenter);
        setAcceptDrops(true);
        updateAppearance(BlockType::EMPTY);
    }

    void updateAppearance(BlockType type) {
        this->type = type;
        switch (type) {
            case BlockType::FLOOR: setStyleSheet("background-color: lightgray"); setText("F"); break;
            case BlockType::WALL: setStyleSheet("background-color: darkgray"); setText("W"); break;
            case BlockType::BOX: setStyleSheet("background-color: sienna"); setText("B"); break;
            case BlockType::PLANT: setStyleSheet("background-color: green"); setText("P"); break;
            default: setStyleSheet(""); setText("");
        }
    }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasText())
            event->acceptProposedAction();
    }

    void dropEvent(QDropEvent* event) override {
        QString block = event->mimeData()->text();
        BlockType droppedType = BlockType::EMPTY;

        if (block == "FLOOR") droppedType = BlockType::FLOOR;
        else if (block == "WALL") droppedType = BlockType::WALL;
        else if (block == "BOX") droppedType = BlockType::BOX;
        else if (block == "PLANT") droppedType = BlockType::PLANT;

        updateAppearance(droppedType);
        emit blockDropped(x, y, droppedType);
    }

signals:
    void blockDropped(int x, int y, BlockType type);

private:
    int x, y;
    BlockType type;
};

// ---------- MapEditor Implementation ----------

MapEditor::MapEditor(QWidget* parent)
    : QWidget(parent), currentMap(nullptr), gridLayout(nullptr), selectedBlockType(BlockType::EMPTY) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    gridWidget = new QWidget(this);
    gridLayout = new QGridLayout(gridWidget);
    gridWidget->setLayout(gridLayout);

    mainLayout->addWidget(gridWidget);
    setupSidebar();
    mainLayout->addWidget(sidebarWidget);
    setLayout(mainLayout);

    // Temporario: Crear mapa de prueba
    currentMap = new MapTemplate("MapaDemo", 10, 10);
    drawGrid();
}

void MapEditor::setupSidebar() {
    sidebarWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(sidebarWidget);
    QLabel* title = new QLabel("Bloques");
    layout->addWidget(title);

    auto addDraggableLabel = [&](const QString& text, BlockType type) {
        QLabel* label = new QLabel(text);
        label->setFixedSize(64, 32);
        label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background-color: lightblue;");
        label->setCursor(Qt::OpenHandCursor);

        label->setMouseTracking(true);
        label->installEventFilter(this);
        label->setProperty("blockType", static_cast<int>(type));
        layout->addWidget(label);
    };

    addDraggableLabel("FLOOR", BlockType::FLOOR);
    addDraggableLabel("WALL", BlockType::WALL);
    addDraggableLabel("BOX", BlockType::BOX);
    addDraggableLabel("PLANT", BlockType::PLANT);

    layout->addStretch();
}

bool MapEditor::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* label = qobject_cast<QLabel*>(watched);
        if (label) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QString text = label->text();
                QMimeData* mimeData = new QMimeData;
                mimeData->setText(text);

                QDrag* drag = new QDrag(label);
                drag->setMimeData(mimeData);
                drag->setPixmap(label->grab());
                drag->exec(Qt::CopyAction);
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MapEditor::drawGrid() {
    // Eliminar celdas anteriores si las hubiera
    QLayoutItem* child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int w = currentMap->getWidth();
    int h = currentMap->getHeight();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            GridCell* cell = new GridCell(x, y);
            connect(cell, &GridCell::blockDropped, this, &MapEditor::handleCellClick);
            gridLayout->addWidget(cell, y, x);
        }
    }
}

void MapEditor::handleCellClick(int x, int y, BlockType type) {
    if (!currentMap) return;
    currentMap->setBlock(x, y, type);
    qDebug() << "Colocado bloque en (" << x << "," << y << "):" << static_cast<int>(type);
}
