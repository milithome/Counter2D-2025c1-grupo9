#include "editorWindow.h"

EditorWindow::EditorWindow(ModoEditor modo, QWidget *parent)
    : QMainWindow(parent), modo(modo)
{
    this->setWindowTitle("Editor de Mapas");
    // Crear stackedWidget y vistas
    stackedWidget = new QStackedWidget(this);
    menuInicialWidget = new QWidget(this);
    editorMapaWidget = new QWidget(this);

    // Agregar las vistas al stackedWidget
    stackedWidget->addWidget(menuInicialWidget);  // índice 0
    stackedWidget->addWidget(editorMapaWidget);   // índice 1
    setCentralWidget(stackedWidget);

    // Configurar la vista inicial según el modo
    stackedWidget->setCurrentIndex(0);
    configurarVistaSegunModo(modo);

}

EditorWindow::~EditorWindow() {}

void EditorWindow::configurarVistaSegunModo(ModoEditor modo) {
    if (modo == CrearNuevoMapa) {
        setupCustomUIConfiguracionMapa();

        // Aplicar estilos responsivos
        aplicarEstilosResponsivos();

        connect(editar_mapa_btn, &QPushButton::clicked, this, &EditorWindow::onCrearMapaClicked);
        connect(volver_menu_btn, &QPushButton::clicked, this, &EditorWindow::onSalirClicked);

        // Aplicar imagen de fondo inicial
        actualizarFondo();

    } else if (modo == EditarMapaExistente) {
        setupCustomUIEleccionMapa();

        // Aplicar estilos responsivos
        aplicarEstilosResponsivos();

        connect(editar_mapa_btn, &QPushButton::clicked, this, &EditorWindow::onEditarMapaClicked);
        connect(volver_menu_btn, &QPushButton::clicked, this, &EditorWindow::onSalirClicked);

        // Aplicar imagen de fondo inicial
        actualizarFondo();

    } else {
        qDebug() << "No se ingreso un modo valido";
    }
}
void EditorWindow::setupCustomUIConfiguracionMapa()
{
    // Crear widget central
    //centralWidget = new QWidget(this);
    //setCentralWidget(centralWidget);

    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget); // usar directamente menuInicialWidget
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    // Spacer superior para centrar verticalmente
    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    // Layout para botones
    QVBoxLayout *btn_line_Layout = new QVBoxLayout();
    btn_line_Layout->setSpacing(20);
    btn_line_Layout->setAlignment(Qt::AlignCenter);

    //Crear la linea donde el usuario pone el nombre y cant de jugadores
    nombre_mapa = new QLineEdit("Nombre del mapa",this);
    cant_jugadores = new QLineEdit("Cant jugadores",this);

    //COnfigurar politica de tamanio para los line editor
    QSizePolicy linePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    nombre_mapa->setSizePolicy(linePolicy);
    cant_jugadores->setSizePolicy(linePolicy);

    // Crear botones con tamaño fijo mínimo pero escalable
    editar_mapa_btn = new QPushButton("Crear mapa", menuInicialWidget);
    volver_menu_btn = new QPushButton("Volver al menu", menuInicialWidget);

    // Configurar política de tamaño para los botones
    QSizePolicy buttonPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    editar_mapa_btn->setSizePolicy(buttonPolicy);
    volver_menu_btn->setSizePolicy(buttonPolicy);

    // Añadir lines y botones al layout
    btn_line_Layout->addWidget(nombre_mapa);
    btn_line_Layout->addWidget(cant_jugadores);
    btn_line_Layout->addWidget(editar_mapa_btn);
    btn_line_Layout->addWidget(volver_menu_btn);

    mainLayout->addLayout(btn_line_Layout);

    // Spacer inferior para centrar verticalmente
    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);
}

void EditorWindow::setupCustomUIEleccionMapa()
{
    // Crear widget central
    //centralWidget = new QWidget(this);
    //setCentralWidget(centralWidget);

    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget); // usar directamente menuInicialWidget
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    // Spacer superior para centrar verticalmente
    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    // Layout para botones
    QVBoxLayout *btn_line_Layout = new QVBoxLayout();
    btn_line_Layout->setSpacing(20);
    btn_line_Layout->setAlignment(Qt::AlignCenter);

    //Crear la linea donde el usuario pone el nombre y cant de jugadores
    nombre_mapa = new QLineEdit("Nombre del mapa",this);

    //COnfigurar politica de tamanio para los line editor
    QSizePolicy linePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    nombre_mapa->setSizePolicy(linePolicy);

    // Crear botones con tamaño fijo mínimo pero escalable
    editar_mapa_btn = new QPushButton("Editar mapa", menuInicialWidget);
    volver_menu_btn = new QPushButton("Volver al menu", menuInicialWidget);

    // Configurar política de tamaño para los botones
    QSizePolicy buttonPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    editar_mapa_btn->setSizePolicy(buttonPolicy);
    volver_menu_btn->setSizePolicy(buttonPolicy);

    // Añadir lines y botones al layout
    btn_line_Layout->addWidget(nombre_mapa);
    btn_line_Layout->addWidget(editar_mapa_btn);
    btn_line_Layout->addWidget(volver_menu_btn);

    mainLayout->addLayout(btn_line_Layout);

    // Spacer inferior para centrar verticalmente
    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);
}

void EditorWindow::aplicarEstilosResponsivos()
{
    // Obtener el tamaño de la pantalla para hacer cálculos responsivos
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    //int screenHeight = screenGeometry.height();

    // Calcular tamaños base según la resolución
    int baseFontSize = qMax(12, screenWidth / 60); // Mínimo 12, escalable
    int lineFontSize = baseFontSize*(2);
    int buttonFontSize = baseFontSize;

    // Configurar fuente del line
    QFont lineFont("Tahoma", lineFontSize, QFont::Bold);
    nombre_mapa->setFont(lineFont);
    if (modo == CrearNuevoMapa) {
        cant_jugadores->setFont(lineFont);
    }
    nombre_mapa->setStyleSheet(
        "QLineEdit {"
        "background-color: rgba(0, 0, 0, 0);"
        "color: yellow;"
        "padding: 25px 20px;"
        "font-size: 30px;"
        "}"
    );
    if (modo == CrearNuevoMapa) {
        cant_jugadores->setStyleSheet(
            "QLineEdit {"
            "background-color: rgba(0, 0, 0, 0);"
            "color: yellow;"
            "padding: 25px 20px;"
            "font-size: 30px;"
            "}"
            );
    }

    // Configurar estilo de botones
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

    editar_mapa_btn->setStyleSheet(buttonStyle);
    volver_menu_btn->setStyleSheet(buttonStyle);
}

void EditorWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    actualizarFondo();

    // Reajustar estilos según el nuevo tamaño
    aplicarEstilosResponsivos();
}

void EditorWindow::actualizarFondo()
{
    QPixmap fondo(":/fondos/fondoSolo.jpg");
    if (!fondo.isNull()) {
        fondo = fondo.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QPalette palette;
        palette.setBrush(QPalette::Window, fondo);
        this->setPalette(palette);
    } else {
        qDebug() << "No se pudo cargar la imagen de fondo desde recursos";
    }
}

void EditorWindow::inicializarEditorMapa() {
    QVBoxLayout* mainLayout = new QVBoxLayout();

    // Parte superior: herramientas
    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    QComboBox* categoriaCombo = new QComboBox();
    categoriaCombo->addItem("Pisos");
    categoriaCombo->addItem("Muros");
    categoriaCombo->addItem("Decoracion");
    toolbarLayout->addWidget(categoriaCombo);

    // Aquí irían los botones/iconos para elegir bloque
    QLabel* icono1 = new QLabel();
    icono1->setPixmap(QPixmap("assets/gfx/backgrounds/aztec.png").scaled(32, 32));
    toolbarLayout->addWidget(icono1);

    QLabel* icono2 = new QLabel();
    icono2->setPixmap(QPixmap("assets/gfx/backgrounds/dust.png").scaled(32, 32));
    toolbarLayout->addWidget(icono2);

    mainLayout->addLayout(toolbarLayout);

    // Grilla central (ejemplo 10x10)
    QGridLayout* gridLayout = new QGridLayout();
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            QLabel* celda = new QLabel();
            celda->setFixedSize(40, 40);
            celda->setStyleSheet("background-color: lightgray; border: 1px solid white;");
            gridLayout->addWidget(celda, i, j);
        }
    }
    mainLayout->addLayout(gridLayout);

    // Parte inferior: botones de acción
    QHBoxLayout* accionesLayout = new QHBoxLayout();
    QPushButton* guardarBtn = new QPushButton("Guardar");
    QPushButton* borrarBtn = new QPushButton("Borrar");
    QPushButton* borrarTodoBtn = new QPushButton("Borrar Todo");
    accionesLayout->addWidget(guardarBtn);
    accionesLayout->addWidget(borrarBtn);
    accionesLayout->addWidget(borrarTodoBtn);

    mainLayout->addLayout(accionesLayout);

    editorMapaWidget->setLayout(mainLayout);
}


void EditorWindow::onCrearMapaClicked() {
    inicializarEditorMapa();
    stackedWidget->setCurrentIndex(1);
}

void EditorWindow::onEditarMapaClicked() {
    inicializarEditorMapa();
    stackedWidget->setCurrentIndex(1);
}

void EditorWindow::onSalirClicked()
{
    this->close();
}