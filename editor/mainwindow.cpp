#include "mainwindow.h"
#include "editorWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setupCustomUI();

    aplicarEstilosResponsivos();

    connect(crearMapaBtn, &QPushButton::clicked, this, &MainWindow::onCrearMapaClicked);
    connect(editarMapaBtn, &QPushButton::clicked, this, &MainWindow::onEditarMapaClicked);
    connect(salirBtn, &QPushButton::clicked, this, &MainWindow::onSalirClicked);

    actualizarFondo();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupCustomUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    tituloLabel = new QLabel("Editor de mapas", centralWidget);
    tituloLabel->setAlignment(Qt::AlignCenter);
    tituloLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainLayout->addWidget(tituloLabel);

    QVBoxLayout *botonesLayout = new QVBoxLayout();
    botonesLayout->setSpacing(20);
    botonesLayout->setAlignment(Qt::AlignCenter);

    crearMapaBtn = new QPushButton("Crear nuevo mapa", centralWidget);
    editarMapaBtn = new QPushButton("Editar mapa", centralWidget);
    salirBtn = new QPushButton("Salir del editor", centralWidget);

    QSizePolicy buttonPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    crearMapaBtn->setSizePolicy(buttonPolicy);
    editarMapaBtn->setSizePolicy(buttonPolicy);
    salirBtn->setSizePolicy(buttonPolicy);

    botonesLayout->addWidget(crearMapaBtn);
    botonesLayout->addWidget(editarMapaBtn);
    botonesLayout->addWidget(salirBtn);

    mainLayout->addLayout(botonesLayout);

    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);
}

void MainWindow::aplicarEstilosResponsivos()
{
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    int baseFontSize = qMax(12, screenWidth / 60); // Mínimo 12, escalable
    int titleFontSize = baseFontSize * 2;
    int buttonFontSize = baseFontSize;

    QFont titleFont("Tahoma", titleFontSize, QFont::Bold);
    tituloLabel->setFont(titleFont);
    tituloLabel->setStyleSheet(
        "QLabel {"
        "background-color: rgba(0, 0, 0, 0);"
        "color: yellow;"
        "padding: 20px;"
        "}"
        );

    QString buttonStyle = QString(
                              "QPushButton {"
                              "color: #ffff66;"
                              "border-radius: 6px;"
                              "padding: 4px;"
                              "font-size: %1px;"
                              "font-weight: bold;"
                              "min-width: 200px;"
                              "min-height: 50px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: rgba(255, 255, 255, 0.05); "
                              "border: 1px solid yellow;"
                              "color: white;"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: rgba(0, 0, 0, 0.3);"
                              "color: #ffaa00;"
                              "}"
                              ).arg(buttonFontSize);

    crearMapaBtn->setStyleSheet(buttonStyle);
    editarMapaBtn->setStyleSheet(buttonStyle);
    salirBtn->setStyleSheet(buttonStyle);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    actualizarFondo();

    aplicarEstilosResponsivos();
}

void MainWindow::actualizarFondo()
{
    QPixmap fondo("/var/taller/gfx/fondoFinal.jpg");
    if (!fondo.isNull()) {
        fondo = fondo.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QPalette palette;
        palette.setBrush(QPalette::Window, fondo);
        this->setPalette(palette);
    } else {
        qDebug() << "No se pudo cargar la imagen de fondo desde recursos";
    }
}

void MainWindow::onCrearMapaClicked()
{
    EditorWindow* editor = new EditorWindow(CrearNuevoMapa);
    editor->show();
}

void MainWindow::onEditarMapaClicked()
{
    EditorWindow* editor = new EditorWindow(EditarMapaExistente);
    editor->show();
}

void MainWindow::onSalirClicked()
{
    this->close();
}
