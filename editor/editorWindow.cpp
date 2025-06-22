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
    stackedWidget->addWidget(menuInicialWidget);  // menu principal == 0
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
    QPixmap fondo(":/assets/gfx/fondoSolo.jpg");
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
    miMenuBar = this->menuBar();

    // Crear menú File
    fileMenu = miMenuBar->addMenu("Archivo");

    // Crear acciones del menú
    guardarAction = new QAction("Guardar", this);
    borrarAction = new QAction("Borrar", this);
    borrarTodoAction = new QAction("Borrar Todo", this);

    // Añadir acciones al menú
    fileMenu->addAction(guardarAction);
    fileMenu->addSeparator();
    fileMenu->addAction(borrarAction);
    fileMenu->addAction(borrarTodoAction);

    // Conectar las acciones a sus slots
    connect(guardarAction, &QAction::triggered, this, &EditorWindow::guardarMapa);
    connect(borrarAction, &QAction::triggered, this, &EditorWindow::borrarSeleccionados);
    connect(borrarTodoAction, &QAction::triggered, this, &EditorWindow::borrarTodo);

    // Layout principal vertical para el editorMapaWidget
    QVBoxLayout *mainLayout = new QVBoxLayout(editorMapaWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Parte superior: herramientas (FIJA)
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(20);
    toolbarLayout->setAlignment(Qt::AlignCenter);

    categoriaCombo = new QComboBox();
    categoriaCombo->addItem("Elegir bloque:");
    categoriaCombo->addItem("Pisos");
    categoriaCombo->addItem("Muros");
    categoriaCombo->addItem("Spawns/Zonas");
    toolbarLayout->addWidget(categoriaCombo);

    // Layout para íconos
    // Layout para íconos (dentro de un contenedor con scroll horizontal)
    iconosLayout = new QHBoxLayout();
    iconosLayout->setSpacing(5);
    iconosLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* iconosContainer = new QWidget();
    iconosContainer->setLayout(iconosLayout);

    QScrollArea* iconosScrollArea = new QScrollArea();
    iconosScrollArea->setWidget(iconosContainer);
    iconosScrollArea->setWidgetResizable(true);
    iconosScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    iconosScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    iconosScrollArea->setFixedHeight(60);  // Ajustá según el alto de tus íconos

    toolbarLayout->addWidget(iconosScrollArea);


    connect(categoriaCombo, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = categoriaCombo->currentText();

        // Limpiar íconos anteriores de forma segura
        limpiarIconosAnteriores();

        if (seleccion == "Pisos") {
            crearIconosPisos();
        }
        else if (seleccion == "Muros") {
            crearIconosMuros();
        }
    });
    // Agregar la barra de herramientas al layout principal (FIJA)
    mainLayout->addLayout(toolbarLayout);

    qDebug() << jugadoresMaximos;

    auto [filas, columnas] = calcularDimensiones();
    //inicializarDatosGrilla(filas, columnas);

    // Inicializar variables de selección
    tipoSeleccionado = VACIO;
    subTipoSeleccionado = 0;

    // CREAR EL SCROLL AREA - CONFIGURACIÓN CORRECTA
    QScrollArea* scrollArea = new QScrollArea();

    // CRÍTICO: widgetResizable debe ser FALSE para mantener tamaños fijos
    scrollArea->setWidgetResizable(false);

    // Configurar políticas de scroll
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Establecer un tamaño máximo para el scroll area para forzar scroll
    scrollArea->setMaximumSize(3000, 3000); // Limitar el tamaño del área visible
    scrollArea->setMinimumSize(400, 300);

    // Widget que contendrá DIRECTAMENTE la grilla
    QWidget* gridWidget = new QWidget();

    // Layout de la grilla - SIN PADDING EXTRA
    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    // CRÍTICO: Política de tamaño fija para evitar redimensionamiento
    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Crear la grilla con tamaños FIJOS
    grillaCeldas.clear();
    grillaCeldas.resize(filas);

    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            QLabel* celda = new QLabel();

            // TAMAÑO FIJO - NO NEGOCIABLE
            celda->setFixedSize(32, 32);
            celda->setMinimumSize(32, 32);
            celda->setMaximumSize(32, 32);

            celda->setStyleSheet(
                "background-color: #f0f0f0; "
                "border: 0.5px solid #999999; "
                "margin: 0px; "
                "padding: 0px;"
                );
            celda->setAlignment(Qt::AlignCenter);

            // Política de tamaño fija
            celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            gridLayout->addWidget(celda, i, j);
            grillaCeldas[i][j] = celda;

            celda->installEventFilter(this);
            celda->setProperty("fila", i);
            celda->setProperty("columna", j);
        }
    }

    // Calcular y establecer el tamaño exacto del widget contenedor
    int anchoTotal = columnas * 32;
    int altoTotal = filas * 32;
    gridWidget->setFixedSize(anchoTotal, altoTotal);

    // Establecer el widget en el scroll area
    scrollArea->setWidget(gridWidget);

    // Centrar el contenido cuando sea más pequeño que el área
    scrollArea->setAlignment(Qt::AlignCenter);

    // Layout para centrar completamente el scrollArea
    QWidget* centroContenedor = new QWidget();
    QGridLayout* centroLayout = new QGridLayout(centroContenedor);
    centroLayout->addWidget(scrollArea, 0, 0, Qt::AlignCenter);
    centroLayout->setContentsMargins(0, 0, 0, 0);
    centroLayout->setSpacing(0);

    // Agregar el contenedor centrado al layout principal
    mainLayout->addWidget(centroContenedor, /* stretch */ 1);

    QHBoxLayout* resizeLayout = new QHBoxLayout();
    resizeLayout->setAlignment(Qt::AlignCenter);

    QLabel* anchoLabel = new QLabel("Ancho visible:");
    QSpinBox* anchoSpin = new QSpinBox();
    anchoSpin->setRange(200, 1000);
    anchoSpin->setValue(scrollArea->width());

    QLabel* altoLabel = new QLabel("Alto visible:");
    QSpinBox* altoSpin = new QSpinBox();
    altoSpin->setRange(200, 780);
    altoSpin->setValue(scrollArea->height());

    resizeLayout->addWidget(anchoLabel);
    resizeLayout->addWidget(anchoSpin);
    resizeLayout->addSpacing(20);
    resizeLayout->addWidget(altoLabel);
    resizeLayout->addWidget(altoSpin);

    mainLayout->addLayout(resizeLayout);

    // CONECTAR LOS SPINBOX A CAMBIOS DINÁMICOS DEL SCROLL AREA
    connect(anchoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int w){
        scrollArea->setFixedWidth(w);
    });
    connect(altoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int h){
        scrollArea->setFixedHeight(h);
    });


    // Parte inferior: botones de acción (FIJOS)
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

    // Agregar los botones de acción al layout principal (FIJOS)
    mainLayout->addLayout(accionesLayout);

    editorMapaWidget->setLayout(mainLayout);
    actualizarEstadoBotonesDimensiones();
}

void EditorWindow::actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado) {
    // Verificar que el nuevo seleccionado no sea nullptr
    if (!nuevoSeleccionado) return;

    // Si había uno seleccionado antes, le saco el borde
    if (iconoSeleccionado && iconosActivos.contains(iconoSeleccionado)) {
        iconoSeleccionado->setStyleSheet("border: 1px solid #999999;");
    }

    // Poner borde al nuevo seleccionado
    nuevoSeleccionado->setStyleSheet("border: 2px solid yellow; background-color: rgba(255, 255, 0, 0.2);");
    iconoSeleccionado = nuevoSeleccionado;
}

void EditorWindow::actualizarTamanoGridWidget() {
    if (grillaCeldas.isEmpty()) return;

    int filas = grillaCeldas.size();
    int columnas = grillaCeldas[0].size();
    int anchoTotal = columnas * 32;
    int altoTotal = filas * 32;

    // Buscar el widget contenedor de la grilla y actualizar su tamaño
    QWidget* gridWidget = gridLayout->parentWidget();
    if (gridWidget) {
        gridWidget->setFixedSize(anchoTotal, altoTotal);
    }
}
// Agregar estas nuevas funciones a la clase EditorWindow:

void EditorWindow::limpiarIconosAnteriores() {
    // Desconectar todas las señales de los íconos actuales
    for (ClickableLabel* icono : iconosActivos) {
        if (icono) {
            disconnect(icono, nullptr, this, nullptr);
        }
    }
    iconosActivos.clear();

    // Limpiar el layout
    QLayoutItem* child;
    while ((child = iconosLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater(); // Usar deleteLater() en lugar de delete
        }
        delete child;
    }

    // Eliminar el iconMapper de forma segura
    if (iconMapper) {
        disconnect(iconMapper, nullptr, this, nullptr);
        iconMapper->deleteLater(); // Usar deleteLater() en lugar de delete
        iconMapper = nullptr;
    }

    // Resetear la selección visual
    iconoSeleccionado = nullptr;
}

void EditorWindow::crearIconosPisos() {
    iconMapper = new QSignalMapper(this);

    QString imagePath = ":/pisos/fondos_32.png";
    QPixmap originalPixmap(imagePath);

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            // Agregar a la lista de íconos activos
            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            // Conectar la señal
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                actualizarSeleccionVisual(icono);
            });
        }
    }
}

void EditorWindow::crearIconosMuros() {
    iconMapper = new QSignalMapper(this);

    QString imagePath = ":/muros/muchos_bloques.png";
    QPixmap originalPixmap(imagePath);

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            // Agregar a la lista de íconos activos
            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            // Conectar la señal
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                actualizarSeleccionVisual(icono);
            });
        }
    }
}

bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* celda = qobject_cast<QLabel*>(obj);
        if (celda && (!pixmapSeleccionado.isNull() || !bloqueSeleccionado.isEmpty())) {
            // Limpiar el contenido anterior de la celda
            celda->clear();

            // Colocar el nuevo bloque
            if (!bloqueSeleccionado.isEmpty()) {
                QPixmap pixmap(bloqueSeleccionado);
                if (!pixmap.isNull()) {
                    celda->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            } else if (!pixmapSeleccionado.isNull()) {
                celda->setPixmap(pixmapSeleccionado.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }

            // Mantener el estilo de la celda
            celda->setAlignment(Qt::AlignCenter);
            return true;
        }
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

        // TAMAÑO FIJO
        celda->setFixedSize(32, 32);
        celda->setMinimumSize(32, 32);
        celda->setMaximumSize(32, 32);
        celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        celda->setStyleSheet(
            "background-color: #f0f0f0; "
            "border: 0.5px solid #999999; "
            "margin: 0px; "
            "padding: 0px;"
            );
        celda->setAlignment(Qt::AlignCenter);
        celda->installEventFilter(this);
        celda->setProperty("fila", nuevasFilas - 1);
        celda->setProperty("columna", j);

        if (!gridLayout) return;
        gridLayout->addWidget(celda, nuevasFilas - 1, j);
        grillaCeldas[nuevasFilas - 1][j] = celda;
    }

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    //redimensionarDatosGrilla();
}

void EditorWindow::agregarColumna() {
    int filas = grillaCeldas.size();
    if (filas == 0) return;

    int nuevasColumnas = grillaCeldas[0].size() + 1;

    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(nuevasColumnas);

        QLabel* celda = new QLabel();

        // TAMAÑO FIJO
        celda->setFixedSize(32, 32);
        celda->setMinimumSize(32, 32);
        celda->setMaximumSize(32, 32);
        celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        celda->setStyleSheet(
            "background-color: #f0f0f0; "
            "border: 0.5px solid #999999; "
            "margin: 0px; "
            "padding: 0px;"
            );
        celda->setAlignment(Qt::AlignCenter);
        celda->installEventFilter(this);
        celda->setProperty("fila", i);
        celda->setProperty("columna", nuevasColumnas - 1);

        if (!gridLayout) return;
        gridLayout->addWidget(celda, i, nuevasColumnas - 1);
        grillaCeldas[i][nuevasColumnas - 1] = celda;
    }

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    //redimensionarDatosGrilla();
}

void EditorWindow::eliminarFila() {
    int filas = grillaCeldas.size();
    if (filas <= 1) return;

    if (!gridLayout) return;
    int columnas = grillaCeldas[0].size();
    for (int j = 0; j < columnas; ++j) {
        QLabel* celda = grillaCeldas[filas - 1][j];
        gridLayout->removeWidget(celda);
        delete celda;
    }

    grillaCeldas.removeLast();

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    //redimensionarDatosGrilla();
}

void EditorWindow::eliminarColumna() {
    if (grillaCeldas.isEmpty() || grillaCeldas[0].size() <= 1) return;

    if (!gridLayout) return;
    int ultimaColumna = grillaCeldas[0].size() - 1;
    for (int i = 0; i < grillaCeldas.size(); ++i) {
        QLabel* celda = grillaCeldas[i][ultimaColumna];
        gridLayout->removeWidget(celda);
        delete celda;
        grillaCeldas[i].removeLast();
    }

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    //redimensionarDatosGrilla();
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
    int base = 30;
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

void EditorWindow::guardarMapa() {
    //guardarMapaEnArchivo();
}
void EditorWindow::borrarSeleccionados(){

}
void EditorWindow::borrarTodo(){

}

/* void EditorWindow::inicializarDatosGrilla(int filas, int columnas) {
    datosGrilla.clear();
    datosGrilla.resize(filas);
    for (int i = 0; i < filas; ++i) {
        datosGrilla[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            datosGrilla[i][j] = InfoCelda(); // Inicializar como VACIO
        }
    }
    qDebug() << "Datos de grilla inicializados:" << filas << "x" << columnas;
} */

// Actualizar información de una celda específica
/* void EditorWindow::actualizarCelda(int fila, int columna, TipoBloque tipo, int subTipo) {
    if (fila >= 0 && fila < datosGrilla.size() &&
        columna >= 0 && columna < datosGrilla[0].size()) {

        datosGrilla[fila][columna].tipo = tipo;
        datosGrilla[fila][columna].subTipo = subTipo;

        qDebug() << "Celda actualizada [" << fila << "," << columna << "]:"
                 << tipoAString(tipo) << "subtipo:" << subTipo;
    }
} */

// Obtener información de una celda
/* InfoCelda EditorWindow::obtenerInfoCelda(int fila, int columna) {
    if (fila >= 0 && fila < datosGrilla.size() &&
        columna >= 0 && columna < datosGrilla[0].size()) {
        return datosGrilla[fila][columna];
    }
    return InfoCelda(); // Retorna celda vacía si está fuera de rango
} */

// Redimensionar la matriz cuando se agregan/eliminan filas/columnas
/* void EditorWindow::redimensionarDatosGrilla() {
    int nuevasFilas = grillaCeldas.size();
    int nuevasColumnas = grillaCeldas.isEmpty() ? 0 : grillaCeldas[0].size();

    // Redimensionar filas
    datosGrilla.resize(nuevasFilas);

    // Redimensionar columnas
    for (int i = 0; i < nuevasFilas; ++i) {
        datosGrilla[i].resize(nuevasColumnas);
    }

    qDebug() << "Datos redimensionados a:" << nuevasFilas << "x" << nuevasColumnas;
} */

// Convertir tipo a string para debug
/* QString EditorWindow::tipoAString(TipoBloque tipo) {
    switch(tipo) {
    case VACIO: return "VACIO";
    case PISO: return "PISO";
    case MURO: return "MURO";
    case SPAWN: return "SPAWN";
    case ZONA: return "ZONA";
    default: return "DESCONOCIDO";
    }
} */

// Mostrar estado actual de la grilla en consola (para debug)
/* void EditorWindow::mostrarEstadoGrillaEnConsola() {
    qDebug() << "=== ESTADO ACTUAL DE LA GRILLA ===";
    for (int i = 0; i < datosGrilla.size(); ++i) {
        QString fila = "Fila " + QString::number(i) + ": ";
        for (int j = 0; j < datosGrilla[i].size(); ++j) {
            InfoCelda info = datosGrilla[i][j];
            fila += "[" + tipoAString(info.tipo) +
                    (info.subTipo > 0 ? ":" + QString::number(info.subTipo) : "") + "] ";
        }
        qDebug() << fila;
    }
    qDebug() << "================================";
} */

/* void EditorWindow::guardarMapaEnArchivo() {
    QString nombreMapa = nombre_mapa->text();
    if (nombreMapa.isEmpty() || nombreMapa == "Nombre del mapa") {
        QMessageBox::warning(this, "Error", "Por favor ingresa un nombre válido para el mapa");
        return;
    }

    QString nombreArchivo = nombreMapa + ".txt";
    QFile archivo(nombreArchivo);

    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo: " + nombreArchivo);
        return;
    }

    QTextStream stream(&archivo);

    // Escribir metadatos
    stream << "MAPA: " << nombreMapa << "\n";
    stream << "JUGADORES: " << jugadoresMaximos << "\n";
    stream << "DIMENSIONES: " << datosGrilla.size() << "x" << (datosGrilla.isEmpty() ? 0 : datosGrilla[0].size()) << "\n";
    stream << "---\n";

    // Escribir datos de la grilla
    for (int i = 0; i < datosGrilla.size(); ++i) {
        for (int j = 0; j < datosGrilla[i].size(); ++j) {
            InfoCelda info = datosGrilla[i][j];
            stream << info.tipo << "," << info.subTipo;
            if (j < datosGrilla[i].size() - 1) stream << " ";
        }
        stream << "\n";
    }

    archivo.close();

    QMessageBox::information(this, "Éxito", "Mapa guardado correctamente: " + nombreArchivo);
    qDebug() << "Mapa guardado en:" << nombreArchivo;

    // Mostrar estado en consola para debug
    mostrarEstadoGrillaEnConsola();
} */

/* void EditorWindow::cargarMapaDesdeArchivo(const QString& nombreArchivo) {
    QFile archivo(nombreArchivo);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo: " + nombreArchivo);
        return;
    }

    QTextStream stream(&archivo);
    QString linea;

    // Leer metadatos
    linea = stream.readLine(); // MAPA: nombre
    if (linea.startsWith("MAPA: ")) {
        QString nombreMapaCargado = linea.mid(6);
        nombre_mapa->setText(nombreMapaCargado);
    }

    linea = stream.readLine(); // JUGADORES: numero
    if (linea.startsWith("JUGADORES: ")) {
        jugadoresMaximos = linea.mid(11).toInt();
        if (cant_jugadores) {
            cant_jugadores->setText(QString::number(jugadoresMaximos));
        }
    }

    linea = stream.readLine(); // DIMENSIONES: filasxcolumnas
    int filas = 0, columnas = 0;
    if (linea.startsWith("DIMENSIONES: ")) {
        QString dimensiones = linea.mid(13);
        QStringList partes = dimensiones.split('x');
        if (partes.size() == 2) {
            filas = partes[0].toInt();
            columnas = partes[1].toInt();
        }
    }

    stream.readLine(); // Leer línea separadora "---"

    // Redimensionar grilla si es necesario
    if (filas > 0 && columnas > 0) {
        // Primero redimensionar la estructura de datos
        datosGrilla.resize(filas);
        for (int i = 0; i < filas; ++i) {
            datosGrilla[i].resize(columnas);
        }

        // Recrear la grilla visual si el tamaño es diferente
        if (grillaCeldas.size() != filas ||
            (grillaCeldas.size() > 0 && grillaCeldas[0].size() != columnas)) {

            // Limpiar grilla actual
            for (int i = 0; i < grillaCeldas.size(); ++i) {
                for (int j = 0; j < grillaCeldas[i].size(); ++j) {
                    if (grillaCeldas[i][j]) {
                        gridLayout->removeWidget(grillaCeldas[i][j]);
                        delete grillaCeldas[i][j];
                    }
                }
            }

            // Recrear grilla con nuevas dimensiones
            grillaCeldas.clear();
            grillaCeldas.resize(filas);

            for (int i = 0; i < filas; ++i) {
                grillaCeldas[i].resize(columnas);
                for (int j = 0; j < columnas; ++j) {
                    QLabel* celda = new QLabel();
                    celda->setFixedSize(32, 32);
                    celda->setMinimumSize(32, 32);
                    celda->setMaximumSize(32, 32);
                    celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                    celda->setStyleSheet(
                        "background-color: #f0f0f0; "
                        "border: 0.5px solid #999999; "
                        "margin: 0px; "
                        "padding: 0px;"
                        );
                    celda->setAlignment(Qt::AlignCenter);
                    celda->installEventFilter(this);
                    celda->setProperty("fila", i);
                    celda->setProperty("columna", j);

                    gridLayout->addWidget(celda, i, j);
                    grillaCeldas[i][j] = celda;
                }
            }

            actualizarTamanoGridWidget();
            actualizarEstadoBotonesDimensiones();
        }
    }

    // Leer y aplicar datos de la grilla
    for (int i = 0; i < filas && !stream.atEnd(); ++i) {
        linea = stream.readLine();
        QStringList celdas = linea.split(' ');

        for (int j = 0; j < columnas && j < celdas.size(); ++j) {
            QStringList partes = celdas[j].split(',');
            if (partes.size() == 2) {
                TipoBloque tipo = static_cast<TipoBloque>(partes[0].toInt());
                int subTipo = partes[1].toInt();

                // Actualizar datos
                datosGrilla[i][j] = InfoCelda(tipo, subTipo);

                // Actualizar visual (necesitarías crear una función que convierta tipo+subtipo a pixmap)
                if (tipo != VACIO && i < grillaCeldas.size() && j < grillaCeldas[i].size()) {
                    QPixmap pixmap = obtenerPixmapParaTipo(tipo, subTipo);
                    if (!pixmap.isNull()) {
                        grillaCeldas[i][j]->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
            }
        }
    }

    archivo.close();

    QMessageBox::information(this, "Éxito", "Mapa cargado correctamente: " + nombreArchivo);
    qDebug() << "Mapa cargado desde:" << nombreArchivo;
    mostrarEstadoGrillaEnConsola();
} */

// FUNCIÓN AUXILIAR PARA OBTENER PIXMAP SEGÚN TIPO (necesitarás implementar esto):

/* QPixmap EditorWindow::obtenerPixmapParaTipo(TipoBloque tipo, int subTipo) {
    // Esta función debería retornar el pixmap correcto según el tipo y subtipo
    // Necesitarás almacenar los pixmaps de alguna manera para poder recuperarlos

    if (tipo == PISO) {
        // Cargar desde la imagen de pisos usando el subtipo
        QString imagePath = ":/pisos/fondos_32.png";
        QPixmap originalPixmap(imagePath);

        int tileWidth = 32;
        int tileHeight = 32;
        int columnas = originalPixmap.width() / tileWidth;

        // Calcular posición basada en subtipo
        int indice = subTipo - 1; // subTipo empieza en 1
        if (indice >= 0) {
            int fila = indice / columnas;
            int columna = indice % columnas;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            return originalPixmap.copy(x, y, tileWidth, tileHeight);
        }
    }
    else if (tipo == MURO) {
        // Similar para muros
        QString imagePath = ":/muros/muchos_bloques.png";
        QPixmap originalPixmap(imagePath);

        int tileWidth = 32;
        int tileHeight = 32;
        int columnas = originalPixmap.width() / tileWidth;

        int indice = subTipo - 1;
        if (indice >= 0) {
            int fila = indice / columnas;
            int columna = indice % columnas;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            return originalPixmap.copy(x, y, tileWidth, tileHeight);
        }
    }

    return QPixmap(); // Retorna pixmap vacío si no se encuentra
} */

// AGREGAR BOTÓN PARA CARGAR EN EL MENÚ:
// En la función inicializarEditorMapa(), agregar:

/* cargarAction = new QAction("Cargar", this);
fileMenu->addAction(cargarAction);
connect(cargarAction, &QAction::triggered, this, [this]() {
   QString nombreArchivo = QFileDialog::getOpenFileName(
       this,
       "Cargar Mapa",
       "",
       "Archivos de Mapa (*.txt)"
       );

   if (!nombreArchivo.isEmpty()) {
       cargarMapaDesdeArchivo(nombreArchivo);
   }
}); */
