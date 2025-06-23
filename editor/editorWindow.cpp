#include "editorWindow.h"

EditorWindow::EditorWindow(ModoEditor modo, QWidget *parent)
    : QMainWindow(parent), modo(modo)
{
    this->setWindowTitle("Editor de Mapas");
    // Crear stackedWidget y vistas
    stackedWidget = new QStackedWidget(this);
    menuInicialWidget = new QWidget(this);
    seleccionSpawnPoints = new QWidget(this);
    editorMapaWidget = new QWidget(this);

    // Agregar las vistas al stackedWidget
    stackedWidget->addWidget(menuInicialWidget);  // menu principal == 0
    stackedWidget->addWidget(seleccionSpawnPoints); // seleccion de spawns == 1
    stackedWidget->addWidget(editorMapaWidget);   // editor (crear o modificar) == 2
    setCentralWidget(stackedWidget);

    // Configurar la vista inicial según el modo
    stackedWidget->setCurrentIndex(0);
    configurarVistaSegunModo(modo);

}


void EditorWindow::configurarVistaSegunModo(ModoEditor modo)
{
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

void EditorWindow::inicializarSpawnsMapa() {
    
    // Layout principal vertical para el editorMapaWidget
    QVBoxLayout *mainLayout = new QVBoxLayout(seleccionSpawnPoints);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Parte superior: herramientas (FIJA)
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(20);
    toolbarLayout->setAlignment(Qt::AlignCenter);

    opciones = new QComboBox();
    opciones->addItem("Elegir bloque:");
    opciones->addItem("Zona de Spawn");
    opciones->addItem("Zona de plantar bomba");
    toolbarLayout->addWidget(opciones);

    // Layout para íconos
    // Layout para íconos (dentro de un contenedor con scroll horizontal)
    iconosLayoutSpawns = new QHBoxLayout();
    iconosLayoutSpawns->setSpacing(5);
    iconosLayoutSpawns->setContentsMargins(0, 0, 0, 0);

    QWidget* iconosContainer = new QWidget();
    iconosContainer->setLayout(iconosLayoutSpawns);

    QScrollArea* iconosScrollArea = new QScrollArea();
    iconosScrollArea->setWidget(iconosContainer);
    iconosScrollArea->setWidgetResizable(true);
    iconosScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    iconosScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    iconosScrollArea->setFixedHeight(50);  // Ajustá según el alto de tus íconos

    toolbarLayout->addWidget(iconosScrollArea);


    connect(opciones, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = opciones->currentText();

        // Limpiar íconos anteriores de forma segura
        limpiarIconosAnteriores(iconosLayoutSpawns, iconosActivosSpawns, iconMapperSpawns, iconoSeleccionadoSpawns);

        if (seleccion == "Zona de Spawn" && jugadoresMaximos > 0) {
            crearIconosSpawns();
        }
        else if (seleccion == "Zona de plantar bomba" && cantZonaPlantable > 0) {
            crearIconosZonaBomba();
        }
    });
    // Agregar la barra de herramientas al layout principal (FIJA)
    mainLayout->addLayout(toolbarLayout);

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
    gridLayoutSpawns = new QGridLayout(gridWidget);
    gridLayoutSpawns->setSpacing(0);
    gridLayoutSpawns->setContentsMargins(0, 0, 0, 0);

    // CRÍTICO: Política de tamaño fija para evitar redimensionamiento
    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Crear la grilla con tamaños FIJOS
    int filas = 20;
    int columnas = 20;
    grillasCeldasSpawns.clear();
    grillasCeldasSpawns.resize(filas);

    for (int i = 0; i < filas; ++i) {
        grillasCeldasSpawns[i].resize(columnas);
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

            gridLayoutSpawns->addWidget(celda, i, j);
            grillasCeldasSpawns[i][j] = celda;

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

    guardarSpawnBtn = new QPushButton("Guardar cambios");
    guardarSpawnBtn->setEnabled(false);

    accionesLayout->addWidget(guardarSpawnBtn);
    mainLayout->addLayout(accionesLayout);

    seleccionSpawnPoints->setLayout(mainLayout);
    actualizarOpcionesDisponibles();
}

void EditorWindow::inicializarEditorMapa(int filas, int columnas) {
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
    iconosScrollArea->setFixedHeight(50);  // Ajustá según el alto de tus íconos

    toolbarLayout->addWidget(iconosScrollArea);


    connect(categoriaCombo, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = categoriaCombo->currentText();

        // Limpiar íconos anteriores de forma segura
        limpiarIconosAnteriores(iconosLayout, iconosActivos, iconMapper, iconoSeleccionado);

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

void EditorWindow::actualizarOpcionesDisponibles() {
    if (!opciones) return;
    
    // Guardar la selección actual
    QString seleccionActual = opciones->currentText();
    
    // Determinar qué opciones deberían estar disponibles
    QStringList opcionesNecesarias;
    opcionesNecesarias << "Elegir bloque:";
    
    if (jugadoresMaximos > 0) {
        opcionesNecesarias << "Zona de Spawn";
    }
    
    if (cantZonaPlantable > 0) {
        opcionesNecesarias << "Zona de plantar bomba";
    }
    
    // Obtener opciones actuales del ComboBox
    QStringList opcionesActuales;
    for (int i = 0; i < opciones->count(); ++i) {
        opcionesActuales << opciones->itemText(i);
    }
    
    // Solo actualizar si las opciones han cambiado
    if (opcionesActuales != opcionesNecesarias) {
        // Bloquear señales para evitar triggers innecesarios
        bool señalesBlocked = opciones->blockSignals(true);
        
        opciones->clear();
        opciones->addItems(opcionesNecesarias);
        
        // Intentar restaurar la selección si sigue siendo válida
        int indiceSeleccion = opciones->findText(seleccionActual);
        if (indiceSeleccion != -1) {
            opciones->setCurrentIndex(indiceSeleccion);
        } else {
            // Si la selección anterior ya no es válida, resetear
            opciones->setCurrentIndex(0);
            limpiarIconosAnteriores(iconosLayoutSpawns, iconosActivosSpawns, iconMapperSpawns, iconoSeleccionadoSpawns);
        }
        
        // Restaurar señales
        opciones->blockSignals(señalesBlocked);
    }
    
    // Verificar si la selección actual sigue siendo válida
    QString nuevaSeleccion = opciones->currentText();
    if ((nuevaSeleccion == "Zona de Spawn" && jugadoresMaximos <= 0) ||
        (nuevaSeleccion == "Zona de plantar bomba" && cantZonaPlantable <= 0)) {
        opciones->setCurrentIndex(0);
        limpiarIconosAnteriores(iconosLayoutSpawns, iconosActivosSpawns, iconMapperSpawns, iconoSeleccionadoSpawns);
    }
    
    // Deshabilitar completamente solo si no hay opciones disponibles
    if (jugadoresMaximos <= 0 && cantZonaPlantable <= 0) {
        opciones->setEnabled(false);
    } else {
        opciones->setEnabled(true);
    }
}

void EditorWindow::actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado, 
                                             QList<ClickableLabel*> iconsActivs, ClickableLabel* iconSelec) {
    // Verificar que el nuevo seleccionado no sea nullptr
    if (!nuevoSeleccionado) return;

    // Si había uno seleccionado antes, le saco el borde
    if (iconSelec && iconsActivs.contains(iconSelec)) {
        iconoSeleccionado->setStyleSheet("border: 1px solid #999999;");
    }

    // Poner borde al nuevo seleccionado
    nuevoSeleccionado->setStyleSheet("border: 2px solid yellow; background-color: rgba(255, 255, 0, 0.2);");
    iconSelec = nuevoSeleccionado;
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
 
void EditorWindow::actualizarJugadoresMaximos(int nuevoValor) {
    jugadoresMaximos = nuevoValor;

    if (jugadoresMaximos == 0) {
        guardarSpawnBtn->setEnabled(true);  // Habilitar el botón
    } else {
        guardarSpawnBtn->setEnabled(false); // (Opcional) Deshabilitar si vuelve a ser > 0
    }
    actualizarOpcionesDisponibles();
}

void EditorWindow::actualizarZonasPlantables(int nuevoValor) {
    cantZonaPlantable = nuevoValor;

    if (cantZonaPlantable == 0) {
        guardarSpawnBtn->setEnabled(true);  // Habilitar el botón
    } else {
        guardarSpawnBtn->setEnabled(false); // (Opcional) Deshabilitar si vuelve a ser > 0
    }
    actualizarOpcionesDisponibles();
}



// Agregar estas nuevas funciones a la clase EditorWindow:

void EditorWindow::limpiarIconosAnteriores(QHBoxLayout*& iconosLay, QList<ClickableLabel*>& iconosActivs, QSignalMapper*& iconMapperSs, ClickableLabel*& iconoSelec) {
    // Desconectar todas las señales de los íconos actuales
    for (ClickableLabel* icono : iconosActivs) {
        if (icono) {
            disconnect(icono, nullptr, this, nullptr);
        }
    }
    iconosActivs.clear();

    // Limpiar el layout
    QLayoutItem* child;
    while ((child = iconosLay->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater(); // Usar deleteLater() en lugar de delete
        }
        delete child;
    }

    // Eliminar el iconMapper de forma segura
    if (iconMapperSs) {
        disconnect(iconMapperSs, nullptr, this, nullptr);
        iconMapperSs->deleteLater(); // Usar deleteLater() en lugar de delete
        iconMapperSs = nullptr;
    }

    // Resetear la selección visual
    iconoSelec = nullptr;
}

void EditorWindow::crearIconosPisos() {
    iconMapper = new QSignalMapper(this);

    QPixmap originalPixmap(":/assets/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},  //pasto
        {1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{2,0},  //arena
        {3,0},{3,1},{3,2},{3,3},  //arena con pastitos
        {3,4},{3,5},{3,6},{3,7},  //arena con cemento
        {4,0},  //cemento
        {5,4},{5,5},{5,6},{5,7}, //baldosas de madera?
        {6,0},{6,1},{6,2}, //arena con piedritas
        {7,0},{7,1}, // A y B zonas
        {9,5},{9,6},{9,7},  //arena mostaza
    };

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            if (!bloquesValidos.contains({fila, columna}))
                continue;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            // MODIFICACIÓN: Capturar las coordenadas del bloque válido
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                coordenadasSeleccionadas = {fila, columna}; // NUEVA LÍNEA
                actualizarSeleccionVisual(icono, iconosActivos, iconoSeleccionado);
            });
        }
    }
}

void EditorWindow::crearIconosMuros() {
    iconMapper = new QSignalMapper(this);

    QPixmap originalPixmap(":/assets/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {2,4},{2,5},{2,6},{2,7},{9,2},{9,3},{9,4},    // cajas
    };

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            if (!bloquesValidos.contains({fila, columna}))
                continue;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            // MODIFICACIÓN: Capturar las coordenadas del bloque válido
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                coordenadasSeleccionadas = {fila, columna}; // NUEVA LÍNEA
                actualizarSeleccionVisual(icono, iconosActivos, iconoSeleccionado);
            });
        }
    }
}

void EditorWindow::crearIconosZonaBomba() {
    iconMapperSpawns = new QSignalMapper(this);

    QPixmap originalPixmap(":/assets/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {4,7}
    };

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            if (!bloquesValidos.contains({fila, columna}))
                continue;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            iconosActivosSpawns.append(icono);
            iconosLayoutSpawns->addWidget(icono);

            // MODIFICACIÓN: Capturar las coordenadas del bloque válido
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionadoSpawns = croppedPixmap;
                coordenadasSeleccionadasSpawns = {fila, columna}; // NUEVA LÍNEA
                actualizarSeleccionVisual(icono, iconosActivosSpawns, iconoSeleccionadoSpawns);
            });
        }
    }
}

void EditorWindow::crearIconosSpawns() {
    iconMapperSpawns = new QSignalMapper(this);

    QPixmap originalPixmap(":/assets/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {0,0}
    };

    int tileWidth = 32;
    int tileHeight = 32;
    int columnas = originalPixmap.width() / tileWidth;
    int filas = originalPixmap.height() / tileHeight;

    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            if (!bloquesValidos.contains({fila, columna}))
                continue;

            int x = columna * tileWidth;
            int y = fila * tileHeight;

            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);

            ClickableLabel* icono = new ClickableLabel();
            icono->setPixmap(croppedPixmap);
            icono->setFrameStyle(QFrame::Box | QFrame::Plain);
            icono->setLineWidth(1);
            icono->setCursor(Qt::PointingHandCursor);

            iconosActivosSpawns.append(icono);
            iconosLayoutSpawns->addWidget(icono);

            // MODIFICACIÓN: Capturar las coordenadas del bloque válido
            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                qDebug() << "Ícono seleccionado para spawn: coordenadas del sprite:"
                 << fila << columna;
                bloqueSeleccionado = "";
                pixmapSeleccionadoSpawns = croppedPixmap;
                coordenadasSeleccionadasSpawns = {fila, columna}; // NUEVA LÍNEA
                actualizarSeleccionVisual(icono, iconosActivosSpawns, iconoSeleccionadoSpawns);
            });
        }
    }
}

bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    
    if (stackedWidget->currentWidget() == seleccionSpawnPoints) {
        if (event->type() == QEvent::MouseButtonPress) {
            QLabel* celda = qobject_cast<QLabel*>(obj);
            if (celda && (!pixmapSeleccionadoSpawns.isNull() || !bloqueSeleccionadoSpawns.isEmpty())) {
                int fila = celda->property("fila").toInt();
                int columna = celda->property("columna").toInt();

                celda->clear();

                if (!bloqueSeleccionadoSpawns.isEmpty()) {
                    // Caso para bloques con string (si lo usas)
                    QPixmap pixmap(bloqueSeleccionadoSpawns);
                    if (!pixmap.isNull()) {
                        celda->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                    // Aquí podrías guardar las coordenadas si tuvieras una variable para bloqueSeleccionado
                } else if (!pixmapSeleccionadoSpawns.isNull()) { // ✅ CORREGIDO: usar pixmapSeleccionadoSpawns
                    // MODIFICACIÓN: Guardar las coordenadas en la matriz
                    if(coordenadasSeleccionadasSpawns.first == 0 && coordenadasSeleccionadasSpawns.second == 0) {
                        matrizGrillaSpawns[fila][columna] = 1; // Zona de spawn
                        actualizarJugadoresMaximos(jugadoresMaximos - 1); // Reducir el contador de jugadores
                        qDebug() << jugadoresMaximos;
                    }
                    else if(coordenadasSeleccionadasSpawns.first == 4 && coordenadasSeleccionadasSpawns.second == 7) {
                        matrizGrillaSpawns[fila][columna] = 2; // Zona de plantar bomba
                        actualizarZonasPlantables(cantZonaPlantable - 1); // Reducir el contador de zonas plantables
                        qDebug() << cantZonaPlantable;
                    }
                    
                    celda->setPixmap(pixmapSeleccionadoSpawns.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    
                    // Debug para verificar que se guarden correctamente
                    qDebug() << "Bloque colocado en (" << fila << "," << columna 
                            << ") con coordenadas del tileset (" 
                            << coordenadasSeleccionadasSpawns.first << "," 
                            << coordenadasSeleccionadasSpawns.second << ")";
                }

                celda->setAlignment(Qt::AlignCenter);
                return true;
            }
        }
        return QMainWindow::eventFilter(obj, event);
        
    } else {
        if (event->type() == QEvent::MouseButtonPress) {
            QLabel* celda = qobject_cast<QLabel*>(obj);
            if (celda && (!pixmapSeleccionado.isNull() || !bloqueSeleccionado.isEmpty())) {
                int fila = celda->property("fila").toInt();
                int columna = celda->property("columna").toInt();

                celda->clear();

                if (!bloqueSeleccionado.isEmpty()) {
                    // Caso para bloques con string (si lo usas)
                    QPixmap pixmap(bloqueSeleccionado);
                    if (!pixmap.isNull()) {
                        celda->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                    // Aquí podrías guardar las coordenadas si tuvieras una variable para bloqueSeleccionado
                } else if (!pixmapSeleccionado.isNull()) {
                    // MODIFICACIÓN: Guardar las coordenadas en la matriz
                    matrizGrilla[fila][columna] = coordenadasSeleccionadas;
                    
                    celda->setPixmap(pixmapSeleccionado.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    
                    // Debug para verificar que se guarden correctamente
                    qDebug() << "Bloque colocado en (" << fila << "," << columna 
                            << ") con coordenadas del tileset (" 
                            << coordenadasSeleccionadas.first << "," 
                            << coordenadasSeleccionadas.second << ")";
                }

                celda->setAlignment(Qt::AlignCenter);
                return true;
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }
}


void EditorWindow::agregarFila() {
    int nuevasFilas = grillaCeldas.size() + 1;
    int columnas = grillaCeldas.isEmpty() ? 0 : grillaCeldas[0].size();

    grillaCeldas.resize(nuevasFilas);
    grillaCeldas[nuevasFilas - 1].resize(columnas);


    // Actualizar matrizGrilla
    matrizGrilla.resize(nuevasFilas);
    matrizGrilla[nuevasFilas - 1].resize(columnas, {0, 0});

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
    if (!rutaArchivoActual.isEmpty()) {
        guardarProgresoEnYaml();
    }
    
}

void EditorWindow::agregarColumna() {
    int filas = grillaCeldas.size();
    if (filas == 0) return;

    int nuevasColumnas = grillaCeldas[0].size() + 1;

    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(nuevasColumnas);

        // Actualizar matrizGrilla
        matrizGrilla[i].resize(nuevasColumnas, {0, 0});

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
    if (!rutaArchivoActual.isEmpty()) {
        guardarProgresoEnYaml();
    }
    
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
    // Actualizar matrizGrilla
    matrizGrilla.removeLast();

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    if (!rutaArchivoActual.isEmpty()) {
        guardarProgresoEnYaml();
    }
    
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
        // Actualizar matrizGrilla
        matrizGrilla[i].removeLast();
    }

    // ACTUALIZAR EL TAMAÑO DEL CONTENEDOR
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    if (!rutaArchivoActual.isEmpty()) {
        guardarProgresoEnYaml();
    }
    
}

void EditorWindow::actualizarEstadoBotonesDimensiones() {
    int filas = grillaCeldas.size();
    int columnas = grillaCeldas[0].size();

    agregarFilaBtn->setEnabled(filas < MAX_FILAS);
    eliminarFilaBtn->setEnabled(filas > MIN_FILAS);

    agregarColumnaBtn->setEnabled(columnas < MAX_COLUMNAS);
    eliminarColumnaBtn->setEnabled(columnas > MIN_COLUMNAS);
}


void EditorWindow::crearArchivoYamlInicial() {
    QString nombreMapa = nombre_mapa->text();

    // Crear el nombre del archivo (sin espacios y con extensión .yaml)
    QString nombreArchivo = nombreMapa;
    nombreArchivo.replace(" ", "_"); // Reemplazar espacios con guiones bajos
    nombreArchivo += ".yaml";

    // Guardar referencias del archivo
    nombreArchivoActual = nombreMapa;
    rutaArchivoActual = QDir::cleanPath("../assets/maps/" + nombreArchivo);

    // Crear el archivo
    QFile archivo(rutaArchivoActual);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo YAML: " + nombreArchivo);
        return;
    }

    QTextStream stream(&archivo);

    // Escribir el contenido inicial del YAML

    int filas = 20; //base
    int columnas = 20;

    stream << "map:" << "\n";
    stream << "  name: " << nombreMapa << "\n";
    stream << "  width: " << columnas << "\n";
    stream << "  height: " << filas << "\n";
    stream << "  background_path: " << "../assets/gfx/backgrounds/sand1.jpg" << "\n";
    stream << "  sprites_path: " << "../assets/gfx/tiles/dust.bmp" << "\n";
    stream << "  players: " << jugadoresMaximos << "\n";
    stream << "\n";
    stream << "  tiles:" << "\n";

    
    matrizGrilla.resize(filas);
    for (int i = 0; i < filas; ++i) {
        matrizGrilla[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            matrizGrilla[i][j] = {0, 0}; // Inicializar con coordenadas vacías
        }
    }

    //inicializo matrizGrilla
    for (int i = 0; i < filas; ++i) {
        if(i!=0){
            stream << "\n"; // Añadir nueva línea para separar filas
        }
        stream << "   - ["; // Indentación para cada fila
        for (int j = 0; j < columnas; ++j) {
            stream << "{" << QString::number(matrizGrilla[i][j].first) 
                    << "," << QString::number(matrizGrilla[i][j].second) << "}";
            if (j < columnas - 1) {
                stream << ","; // Mantener el formato correcto
            }
        }
        if(i < filas){
            stream << "]" << "\n "; // Cerrar la fila
        }
    }

    stream << "\n";
    stream << "  game:" << "\n";

    //incializo la matriz de spawns (game)
    matrizGrillaSpawns.resize(filas);
    for (int i = 0; i < filas; ++i) {
        matrizGrillaSpawns[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            matrizGrillaSpawns[i][j] = 0; // Inicializar con coordenadas vacías
        }
    }

    for (int i = 0; i < filas; ++i) {
        if(i!=0){
            stream << "\n"; // Añadir nueva línea para separar filas
        }
        stream << "   - ["; // Indentación para cada fila
        for (int j = 0; j < columnas; ++j) {
            stream << matrizGrillaSpawns[i][j];
            if (j < columnas - 1) {
                stream << ","; // Mantener el formato correcto
            }
        }
        if(i < filas){
            stream << "]" << "\n "; // Cerrar la fila
        }
    }

    stream << "\n";
    stream << "  legend_tiles:" << "\n";

    stream << "     1:" << "\n"; // 1 es el muro
    stream << "     name: \"Cajas\"" << "\n";
    stream << "         {2,4}" << "\n" << // {2,4} es la caja
            "           {2,5}" << "\n" << 
            "           {2,6}" << "\n" << 
            "           {2,7}" << "\n" << 
            "           {9,2}" << "\n" << 
            "           {9,3}" << "\n" << 
            "           {9,4}" << "\n"; 

    stream << "     2:" << "\n"; // 2 es el piso
    stream << "     name: \"Pasto\"" << "\n";
    stream << "         {0,1}" << "\n" << 
            "           {0,2}" << "\n" << 
            "           {0,3}" << "\n" << 
            "           {0,4}" << "\n" << 
            "           {0,5}" << "\n" << 
            "           {0,6}" << "\n" << 
            "           {0,7}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena\"" << "\n";
    stream << "         {1,0}" << "\n" << 
            "           {1,1}" << "\n" << 
            "           {1,2}" << "\n" << 
            "           {1,3}" << "\n" << 
            "           {1,4}" << "\n" << 
            "           {1,5}" << "\n" << 
            "           {1,6}" << "\n" << 
            "           {1,7}" << "\n" << 
            "           {2,0}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena con pastitos\"" << "\n";
    stream << "         {3,0}" << "\n" << 
            "           {3,1}" << "\n" <<
            "           {3,2}" << "\n" <<
            "           {3,3}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena con cemento\"" << "\n";
    stream << "         {3,4}" << "\n" << 
            "           {3,5}" << "\n" <<
            "           {3,6}" << "\n" <<
            "           {3,7}" << "\n"; // <-- CORREGIDO

    stream << "     name: \"Cemento\"" << "\n";
    stream << "         {4,0}" << "\n"; // {4,0} es el cemento

    stream << "     name: \"Baldosas de madera\"" << "\n";
    stream << "         {5,4}" << "\n" << 
            "           {5,5}" << "\n" <<
            "           {5,6}" << "\n" <<
            "           {5,7}" << "\n";

    stream << "     name: \"A y B en rojo\"" << "\n";
    stream << "         {7,0}" << "\n" << 
            "           {7,1}" << "\n";

    stream << "     name: \"Arena mostaza\"" << "\n";
    stream << "         {9,5}" << "\n" <<
            "           {9,6}" << "\n" <<
            "           {9,7}" << "\n";



    archivo.close();

    QMessageBox::information(this, "Archivo Creado",
                             "Archivo YAML creado: " + nombreArchivo + "\n" +
                                 "Ubicación: " + rutaArchivoActual);

    qDebug() << "Archivo YAML inicial creado en:" << rutaArchivoActual;
}


void EditorWindow::guardarProgresoEnYaml() {
    QFile archivo(rutaArchivoActual);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo YAML para guardar");
        return;
    }

    QTextStream stream(&archivo);

    // Obtener dimensiones actuales
    int filas = datosMapa.filas;
    int columnas = datosMapa.columnas;

    // Escribir encabezado YAML
    stream << "map:" << "\n";
    stream << "  name: " << nombreArchivoActual << "\n";
    stream << "  width: " << columnas << "\n";
    stream << "  height: " << filas << "\n";
    stream << "  background_path: " << "../assets/gfx/backgrounds/sand1.jpg" << "\n";
    stream << "  sprites_path: " << "../assets/gfx/tiles/dust.bmp" << "\n";
    stream << "  players: " << jugadoresMaximos << "\n";
    stream << "\n";
    stream << "  tiles:" << "\n";

    //inicializo matrizGrilla
    for (int i = 0; i < filas; ++i) {
        if(i!=0){
            stream << "\n"; // Añadir nueva línea para separar filas
        }
        stream << "   - ["; // Indentación para cada fila
        for (int j = 0; j < columnas; ++j) {
            stream << "{" << QString::number(matrizGrilla[i][j].first) 
                    << "," << QString::number(matrizGrilla[i][j].second) << "}";
            if (j < columnas - 1) {
                stream << ","; // Mantener el formato correcto
            }
        }
        if(i < filas){
            stream << "]" << "\n "; // Cerrar la fila
        }
    }

    stream << "\n";
    stream << "  game:" << "\n";

    for (int i = 0; i < filas; ++i) {
        if(i!=0){
            stream << "\n"; // Añadir nueva línea para separar filas
        }
        stream << "   - ["; // Indentación para cada fila
        for (int j = 0; j < columnas; ++j) {
            stream << matrizGrillaSpawns[i][j];
            if (j < columnas - 1) {
                stream << ","; // Mantener el formato correcto
            }
        }
        if(i < filas){
            stream << "]" << "\n "; // Cerrar la fila
        }
    }

    stream << "\n";
    stream << "  legend_tiles:" << "\n";

    stream << "     1:" << "\n"; // 1 es el muro
    stream << "     name: \"Cajas\"" << "\n";
    stream << "         {2,4}" << "\n" << // {2,4} es la caja
            "           {2,5}" << "\n" << 
            "           {2,6}" << "\n" << 
            "           {2,7}" << "\n" << 
            "           {9,2}" << "\n" << 
            "           {9,3}" << "\n" << 
            "           {9,4}" << "\n"; 

    stream << "     2:" << "\n"; // 2 es el piso
    stream << "     name: \"Pasto\"," << "\n";
    stream << "         {0,1}" << "\n" << 
            "           {0,2}" << "\n" << 
            "           {0,3}" << "\n" << 
            "           {0,4}" << "\n" << 
            "           {0,5}" << "\n" << 
            "           {0,6}" << "\n" << 
            "           {0,7}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena\"" << "\n";
    stream << "         {1,0}" << "\n" << 
            "           {1,1}" << "\n" << 
            "           {1,2}" << "\n" << 
            "           {1,3}" << "\n" << 
            "           {1,4}" << "\n" << 
            "           {1,5}" << "\n" << 
            "           {1,6}" << "\n" << 
            "           {1,7}" << "\n" << 
            "           {2,0}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena con pastitos\"" << "\n";
    stream << "         {3,0}" << "\n" << 
            "           {3,1}" << "\n" <<
            "           {3,2}" << "\n" <<
            "           {3,3}" << "\n"; // <-- CORREGIDO: faltaba ;

    stream << "     name: \"Arena con cemento\"" << "\n";
    stream << "         {3,4}" << "\n" << 
            "           {3,5}" << "\n" <<
            "           {3,6}" << "\n" <<
            "           {3,7}" << "\n"; // <-- CORREGIDO

    stream << "     name: \"Cemento\"" << "\n";
    stream << "         {4,0}" << "\n"; // {4,0} es el cemento

    stream << "     name: \"Baldosas de madera\"" << "\n";
    stream << "         {5,4}" << "\n" << 
            "           {5,5}" << "\n" <<
            "           {5,6}" << "\n" <<
            "           {5,7}" << "\n";

    stream << "     name: \"A y B en rojo\"" << "\n";
    stream << "         {7,0}" << "\n" << 
            "           {7,1}" << "\n";

    stream << "     name: \"Arena mostaza\"" << "\n";
    stream << "         {9,5}" << "\n" <<
            "           {9,6}" << "\n" <<
            "           {9,7}" << "\n";

    
    

    archivo.close();

    QMessageBox::information(this, "Guardado Exitoso",
                             "Mapa guardado correctamente en: " + rutaArchivoActual);

    qDebug() << "Progreso guardado en:" << rutaArchivoActual;
}



void EditorWindow::onCrearMapaClicked() {

    QString name = nombre_mapa->text();

    // Validar que el nombre no esté vacío y no sea el placeholder
    if (name.isEmpty() || name == "Nombre del mapa") {
        QMessageBox::warning(this, "Error", "Por favor ingresa un nombre válido para el mapa");
        return;
    }

    QString textoCantJugadores = cant_jugadores->text();
    jugadoresMaximos = textoCantJugadores.toInt();
    cantZonaPlantable = 4;

    // Validar cantidad de jugadores
    if (jugadoresMaximos <= 0 || jugadoresMaximos > 10) {
        QMessageBox::warning(this, "Error", "La cantidad de jugadores debe ser entre 1 y 10");
        return;
    }

    jugadoresMaximos = jugadoresMaximos*2;

    // Crear el archivo YAML inicial
    crearArchivoYamlInicial();

    inicializarSpawnsMapa();

    datosMapa = {
        name,     // nombreMapa
        20,              // filas
        20,              // columnas
        jugadoresMaximos,               // playersMax
        4,               // cantZonaPlantable
        matrizGrilla, // matrizGrilla
        matrizGrillaSpawns               // matrizGrillaSpawns
    };

    // Conectar los botones a sus slots
    connect(guardarSpawnBtn, &QPushButton::clicked, this, &EditorWindow::onGuardarSpawnMapa);
    
    stackedWidget->setCurrentIndex(1);
}

void EditorWindow::onEditarMapaClicked() {
    QString nombreMapa = nombre_mapa->text();

    if (nombreMapa.isEmpty() || nombreMapa == "Nombre del mapa") {
        QMessageBox::warning(this, "Error", "Por favor ingresa un nombre válido para el mapa");
        return;
    }

    // Buscar el archivo YAML
    QString nombreArchivo = nombreMapa;
    nombreArchivo.replace(" ", "_");
    nombreArchivo += ".yaml";

    QString rutaArchivo = QDir::cleanPath("../assets/maps/" + nombreArchivo);

    if (!QFile::exists(rutaArchivo)) {
        QMessageBox::warning(this, "Error", "No se encontró el archivo: " + nombreArchivo);
        return;
    }

    // Configurar referencias del archivo
    nombreArchivoActual = nombreMapa;
    rutaArchivoActual = rutaArchivo;

    //inicializarEditorMapa();
    stackedWidget->setCurrentIndex(1);

    QMessageBox::information(this, "Mapa Cargado",
                             "Mapa cargado: " + nombreArchivo);
}


void EditorWindow::onSalirClicked()
{
    this->close();
}

void EditorWindow::onGuardarSpawnMapa()
{
    guardarProgresoEnYaml();
    int filas = grillasCeldasSpawns.size();
    int columnas = grillasCeldasSpawns.isEmpty() ? 0 : grillasCeldasSpawns[0].size();

    inicializarEditorMapa(filas,columnas);
    stackedWidget->setCurrentIndex(2);

}

EditorWindow::~EditorWindow()
{

}

void EditorWindow::guardarMapa() {
    if (rutaArchivoActual.isEmpty()) {
        QMessageBox::warning(this, "Error", "No hay archivo YAML asociado a este mapa");
        return;
    }

    guardarProgresoEnYaml();
}
void EditorWindow::borrarSeleccionados(){

}
void EditorWindow::borrarTodo(){

}