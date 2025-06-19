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
    stackedWidget->addWidget(menuInicialWidget);  // menu principal ==0
    stackedWidget->addWidget(editorMapaWidget);   // editor (crear o modificar) == 1
    setCentralWidget(stackedWidget);

    // Configurar la vista inicial según el modo
    stackedWidget->setCurrentIndex(0);
    configurarVistaSegunModo(modo);

}

void EditorWindow::configurarVistaSegunModo(ModoEditor modo) {
    if (modo == CrearNuevoMapa) {
        setupCustomUIConfiguracionMapa();


        aplicarEstilosResponsivos();
        connect(editar_mapa_btn, &QPushButton::clicked, this, &EditorWindow::onCrearMapaClicked);
        connect(volver_menu_btn, &QPushButton::clicked, this, &EditorWindow::onSalirClicked);


        actualizarFondo();

    } else if (modo == EditarMapaExistente) {
        setupCustomUIEleccionMapa();


        aplicarEstilosResponsivos();

        connect(editar_mapa_btn, &QPushButton::clicked, this, &EditorWindow::onEditarMapaClicked);
        connect(volver_menu_btn, &QPushButton::clicked, this, &EditorWindow::onSalirClicked);


        actualizarFondo();

    } else {
        qDebug() << "No se ingreso un modo valido";
    }
}
void EditorWindow::setupCustomUIConfiguracionMapa()
{
    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget);
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
    cant_jugadores = new QLineEdit(this);
    cant_jugadores->setValidator(new QIntValidator(1, 10, this));

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
    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget);
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


    // CREAR LA BARRA DE MENÚ
    miMenuBar = this->menuBar(); // Usar la barra de menú nativa de QMainWindow

    // Crear menú File
    fileMenu = miMenuBar->addMenu("Archivo");

    // Crear acciones del menú
    guardarAction = new QAction("Guardar", this);
    borrarAction = new QAction("Borrar", this);
    borrarTodoAction = new QAction("Borrar Todo", this);

    // Añadir acciones al menú
    fileMenu->addAction(guardarAction);
    fileMenu->addSeparator(); // Separador visual
    fileMenu->addAction(borrarAction);
    fileMenu->addAction(borrarTodoAction);

    // Conectar las acciones a sus slots
    connect(guardarAction, &QAction::triggered, this, &EditorWindow::guardarMapa);
    connect(borrarAction, &QAction::triggered, this, &EditorWindow::borrarSeleccionados);
    connect(borrarTodoAction, &QAction::triggered, this, &EditorWindow::borrarTodo);


    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(editorMapaWidget);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    // Spacer superior para centrar verticalmente
    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    // Parte superior: herramientas
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(20);
    toolbarLayout->setAlignment(Qt::AlignCenter);

    categoriaCombo = new QComboBox();
    categoriaCombo->addItem("Elegir bloque:");
    categoriaCombo->addItem("Pisos");
    categoriaCombo->addItem("Muros");
    categoriaCombo->addItem("Spawns/Zonas");
    toolbarLayout->addWidget(categoriaCombo);

    // Layout para íconos, separado y accesible globalmente
    iconosLayout = new QHBoxLayout();
    toolbarLayout->addLayout(iconosLayout);  // agregamos el layout vacío al principio


    connect(categoriaCombo, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = categoriaCombo->currentText();

        // Limpiar íconos anteriores
        QLayoutItem* child;
        while ((child = iconosLayout->takeAt(0)) != nullptr) {
            if (child->widget()) delete child->widget();
            delete child;
        }

        if (iconMapper) {
            delete iconMapper;
            iconMapper = nullptr;
        }

        if (seleccion == "Pisos") {
            iconMapper = new QSignalMapper(this);

            QStringList iconPaths = {
                ":/pisos/aztec.png",
                ":/pisos/dust.png",
                ":/pisos/gras1.png",
                ":/pisos/blood.png"
            };

            for (const QString& path : iconPaths) {
                ClickableLabel* icono = new ClickableLabel();

                icono->setPixmap(QPixmap(path).scaled(32, 32));
                icono->setFrameStyle(QFrame::Box | QFrame::Plain);
                icono->setLineWidth(1);
                icono->setCursor(Qt::PointingHandCursor);
                iconosLayout->addWidget(icono);

                connect(icono, &ClickableLabel::clicked, this, [this, path, icono]() {
                    bloqueSeleccionado = path;
                    actualizarSeleccionVisual(icono);
                });
            }
        }
    });


    mainLayout->addLayout(toolbarLayout);

    qDebug() << jugadoresMaximos;

    auto [filas, columnas] = calcularDimensiones();

    // Grilla central
    gridLayout = new QGridLayout();
    grillaCeldas.clear();
    grillaCeldas.resize(filas);

    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            QLabel* celda = new QLabel();
            celda->setFixedSize(32, 32);
            celda->setStyleSheet("background-color: lightgray; border: 1px solid white;");
            celda->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(celda, i, j);
            grillaCeldas[i][j] = celda;

            // Hacer clickeable la celda
            celda->installEventFilter(this);
            celda->setProperty("fila", i);
            celda->setProperty("columna", j);
        }
    }

    mainLayout->addLayout(gridLayout);

    // Parte inferior: botones de acción
    QHBoxLayout* accionesLayout = new QHBoxLayout();

    // Botones para agregar fila/columna
    agregarFilaBtn = new QPushButton("Agregar fila");
    agregarColumnaBtn = new QPushButton("Agregar columna");

    accionesLayout->addWidget(agregarFilaBtn);
    accionesLayout->addWidget(agregarColumnaBtn);

    // Conectar los botones a sus slots
    connect(agregarFilaBtn, &QPushButton::clicked, this, &EditorWindow::agregarFila);
    connect(agregarColumnaBtn, &QPushButton::clicked, this, &EditorWindow::agregarColumna);

    eliminarFilaBtn = new QPushButton("Eliminar fila");
    eliminarColumnaBtn = new QPushButton("Eliminar columna");

    accionesLayout->addWidget(eliminarFilaBtn);
    accionesLayout->addWidget(eliminarColumnaBtn);

    connect(eliminarFilaBtn, &QPushButton::clicked, this, &EditorWindow::eliminarFila);
    connect(eliminarColumnaBtn, &QPushButton::clicked, this, &EditorWindow::eliminarColumna);

    mainLayout->addLayout(accionesLayout);

    // Spacer inferior para centrar verticalmente
    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);

    editorMapaWidget->setLayout(mainLayout);
    actualizarEstadoBotonesDimensiones();

}

void EditorWindow::actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado)
{
    // Si había uno seleccionado antes, le saco el borde
    if (iconoSeleccionado) {
        iconoSeleccionado->setStyleSheet("background-color: lightgray; border: 1px solid white;"); // o el estilo que tenía por defecto
    }

    // Poner borde al nuevo seleccionado
    nuevoSeleccionado->setStyleSheet("border: 2px solid yellow;");
    iconoSeleccionado = nuevoSeleccionado;
}


bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* celda = qobject_cast<QLabel*>(obj);
        if (celda && !bloqueSeleccionado.isEmpty()) {
            celda->setPixmap(QPixmap(bloqueSeleccionado).scaled(32, 32));
        }
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void EditorWindow::agregarFila() {
    int nuevasFilas = grillaCeldas.size() + 1;
    int columnas = grillaCeldas.isEmpty() ? 0 : grillaCeldas[0].size();

    grillaCeldas.resize(nuevasFilas);
    grillaCeldas[nuevasFilas - 1].resize(columnas);

    for (int j = 0; j < columnas; ++j) {
        QLabel* celda = new QLabel();
        celda->setFixedSize(32, 32);
        celda->setStyleSheet("background-color: lightgray; border: 1px solid white;");
        celda->setAlignment(Qt::AlignCenter);
        celda->installEventFilter(this);
        celda->setProperty("fila", nuevasFilas - 1);
        celda->setProperty("columna", j);

        if (!gridLayout) return; // Por seguridad
        gridLayout->addWidget(celda, nuevasFilas - 1, j);
        grillaCeldas[nuevasFilas - 1][j] = celda;
    }
    actualizarEstadoBotonesDimensiones();

}

void EditorWindow::guardarMapa(){

}
void EditorWindow::borrarSeleccionados(){

}
void EditorWindow::borrarTodo(){

}

void EditorWindow::agregarColumna() {
    int filas = grillaCeldas.size();
    if (filas == 0) return;

    int nuevasColumnas = grillaCeldas[0].size() + 1;

    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(nuevasColumnas);

        QLabel* celda = new QLabel();
        celda->setFixedSize(32, 32);
        celda->setStyleSheet("background-color: lightgray; border: 1px solid white;");
        celda->setAlignment(Qt::AlignCenter);
        celda->installEventFilter(this);
        celda->setProperty("fila", i);
        celda->setProperty("columna", nuevasColumnas - 1);

        if (!gridLayout) return; // Por seguridad
        gridLayout->addWidget(celda, i, nuevasColumnas - 1);
        grillaCeldas[i][nuevasColumnas - 1] = celda;
    }
    actualizarEstadoBotonesDimensiones();

}

void EditorWindow::eliminarFila() {
    int filas = grillaCeldas.size();
    if (filas <= 1) return; // Evitamos dejar 0 filas

    if (!gridLayout) return; // Por seguridad
    int columnas = grillaCeldas[0].size();
    for (int j = 0; j < columnas; ++j) {
        QLabel* celda = grillaCeldas[filas - 1][j];
        gridLayout->removeWidget(celda);
        delete celda;
    }

    grillaCeldas.removeLast();
    actualizarEstadoBotonesDimensiones();

}

void EditorWindow::eliminarColumna() {
    if (grillaCeldas.isEmpty() || grillaCeldas[0].size() <= 1) return;

    if (!gridLayout) return; // Por seguridad
    int ultimaColumna = grillaCeldas[0].size() - 1;
    for (int i = 0; i < grillaCeldas.size(); ++i) {
        QLabel* celda = grillaCeldas[i][ultimaColumna];
        gridLayout->removeWidget(celda);
        delete celda;
        grillaCeldas[i].removeLast();
    }
    actualizarEstadoBotonesDimensiones();

}

void EditorWindow::actualizarEstadoBotonesDimensiones() {
    int filas = grillaCeldas.size();
    int columnas = grillaCeldas[0].size();

    agregarFilaBtn->setEnabled(filas < MAX_FILAS);
    eliminarFilaBtn->setEnabled(filas > MIN_FILAS);

    agregarColumnaBtn->setEnabled(columnas < MAX_COLUMNAS);
    eliminarColumnaBtn->setEnabled(columnas > MIN_COLUMNAS);
}



std::pair<int, int> EditorWindow::calcularDimensiones() {
    int base = 10;
    int aumento = 0;
    return {base + aumento, base + aumento};
}


void EditorWindow::onCrearMapaClicked() {
    QString texto = cant_jugadores->text();
    jugadoresMaximos = texto.toInt();
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

EditorWindow::~EditorWindow()
{

}
