#include "editorWindow.h"

EditorWindow::EditorWindow(ModoEditor modo, QWidget *parent)
    : QMainWindow(parent), modo(modo)
{
    this->setWindowTitle("Editor de Mapas");

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QString texto;
    if (modo == CrearNuevoMapa) {
        texto = "Modo: Crear nuevo mapa";
    } else {
        texto = "Modo: Editar mapa existente";
    }

    QLabel* label = new QLabel(texto, this);
    layout->addWidget(label);

    this->setCentralWidget(central);

    configurarVistaSegunModo();
}

EditorWindow::~EditorWindow() {}

void EditorWindow::configurarVistaSegunModo() {
    // Acá agregarás la lógica específica según el modo
}
