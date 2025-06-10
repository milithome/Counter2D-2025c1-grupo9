#include "editorMenuWindow.h"

EditorMenuWindow::EditorMenuWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Editor");

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;

    QLabel *titulo = new QLabel("Editor de Mapas");
    titulo->setAlignment(Qt::AlignCenter); // Centrar texto
    titulo->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(titulo);

    // Crear los botones
    nuevoMapaBtn = new QPushButton("Nuevo Mapa");
    mapasGuardadosBtn = new QPushButton("Mapas Guardados");
    volverMenuBtn = new QPushButton("Volver al Menú");

    // Configurar tamaño de los botones
    nuevoMapaBtn->setMinimumHeight(50);
    nuevoMapaBtn->setMaximumSize(300, 60);
    mapasGuardadosBtn->setMinimumHeight(50);
    mapasGuardadosBtn->setMaximumSize(300, 60);
    volverMenuBtn->setMinimumHeight(50);
    volverMenuBtn->setMaximumSize(300, 60);

    // Alinear los botones al centro
    layout->setAlignment(Qt::AlignCenter);

    // Añadir los botones al layout
    layout->addWidget(nuevoMapaBtn);
    layout->addWidget(mapasGuardadosBtn);
    layout->addWidget(volverMenuBtn);

    // Establecer el fondo del layout
    /* QPalette palette;
    QPixmap background("/home/milito/Desktop/TALLER I/Tpg/Counter2D-2025c1-grupo9/build/editor/fotoPantallaCS.jpg");
    if (!background.isNull()) {
        palette.setBrush(QPalette::Window, background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        qWarning("Failed to load background image.");
    }
    central->setAutoFillBackground(true);
    central->setPalette(palette); */

    central->setLayout(layout);
    setCentralWidget(central);

    connect(nuevoMapaBtn, &QPushButton::clicked, this, [this]() {
        std::string nombre = "NuevoMapa";
        int ancho = 15;
        int alto = 10;
        QWidget* parent;

        MapEditor* editor = new MapEditor(parent, nombre, ancho, alto);
        editor->show();
    });
    connect(volverMenuBtn, &QPushButton::clicked, this, &QMainWindow::close);
}
