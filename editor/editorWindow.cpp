#include "editorWindow.h"

EditorWindow::EditorWindow(ModoEditor modo, QWidget *parent)
    : QMainWindow(parent), modo(modo)
{
    this->setWindowTitle("Editor de Mapas");
    stackedWidget = new QStackedWidget(this);
    menuInicialWidget = new QWidget(this);
    seleccionSpawnPoints = new QWidget(this);
    editorMapaWidget = new QWidget(this);
    editorMapaExistenteWidget = new QWidget(this);

    stackedWidget->addWidget(menuInicialWidget);  // menu principal == 0
    stackedWidget->addWidget(seleccionSpawnPoints); // seleccion de spawns == 1
    stackedWidget->addWidget(editorMapaWidget);   // editor (crear o modificar) == 2
    stackedWidget->addWidget(editorMapaExistenteWidget); // editor de mapa existente == 3
    setCentralWidget(stackedWidget);

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

        QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
        connect(enterShortcut, &QShortcut::activated, editar_mapa_btn, &QPushButton::click);

        actualizarFondo();

    } else if (modo == EditarMapaExistente) {
        setupCustomUIEleccionMapa();

        aplicarEstilosResponsivos();
        
        connect(editar_mapa_btn, &QPushButton::clicked, this, &EditorWindow::onEditarMapaClicked);
        connect(volver_menu_btn, &QPushButton::clicked, this, &EditorWindow::onSalirClicked);

        QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
        connect(enterShortcut, &QShortcut::activated, editar_mapa_btn, &QPushButton::click);

        actualizarFondo();

    } else {
        qDebug() << "No se ingreso un modo valido";
    }
}

void EditorWindow::setupCustomUIConfiguracionMapa()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    QVBoxLayout *btn_line_Layout = new QVBoxLayout();
    btn_line_Layout->setSpacing(20);
    btn_line_Layout->setAlignment(Qt::AlignCenter);

    nombre_mapa = new QLineEdit("Nombre del mapa",this);
    nombre_mapa->installEventFilter(this);
    cant_jugadores = new QLineEdit(this);
    cant_jugadores->setValidator(new QIntValidator(1, 10, this));

    QSizePolicy linePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    nombre_mapa->setSizePolicy(linePolicy);
    cant_jugadores->setSizePolicy(linePolicy);

    editar_mapa_btn = new QPushButton("Crear mapa", menuInicialWidget);
    volver_menu_btn = new QPushButton("Volver al menu", menuInicialWidget);

    QSizePolicy buttonPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    editar_mapa_btn->setSizePolicy(buttonPolicy);
    volver_menu_btn->setSizePolicy(buttonPolicy);

    tituloCantJugadores = new QLabel("Cantidad de jugadores (1-10):", this);
    tituloCantJugadores->setStyleSheet("background-color: rgba(0, 0, 0, 0); color: yellow; padding: 25px 20px; font-size: 30px;");
    tituloCantJugadores->setAlignment(Qt::AlignCenter);

    
    btn_line_Layout->addWidget(nombre_mapa);
    btn_line_Layout->addWidget(tituloCantJugadores);
    btn_line_Layout->addWidget(cant_jugadores);

    btn_line_Layout->addWidget(editar_mapa_btn);
    btn_line_Layout->addWidget(volver_menu_btn);

    mainLayout->addLayout(btn_line_Layout);

    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);
}

void EditorWindow::setupCustomUIEleccionMapa()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(menuInicialWidget);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);

    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(topSpacer);

    QVBoxLayout *btn_line_Layout = new QVBoxLayout();
    btn_line_Layout->setSpacing(20);
    btn_line_Layout->setAlignment(Qt::AlignCenter);

    mapasExistentes = new QComboBox(this);

    cargarArchivosYamlEnComboBox(mapasExistentes);

    QSizePolicy comboPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mapasExistentes->setSizePolicy(comboPolicy);

    editar_mapa_btn = new QPushButton("Editar mapa", menuInicialWidget);
    volver_menu_btn = new QPushButton("Volver al menu", menuInicialWidget);

    QSizePolicy buttonPolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    editar_mapa_btn->setSizePolicy(buttonPolicy);
    volver_menu_btn->setSizePolicy(buttonPolicy);

    btn_line_Layout->addWidget(mapasExistentes);
    btn_line_Layout->addWidget(editar_mapa_btn);
    btn_line_Layout->addWidget(volver_menu_btn);

    mainLayout->addLayout(btn_line_Layout);

    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);
}

void EditorWindow::aplicarEstilosResponsivos()
{
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();

    int baseFontSize = qMax(12, screenWidth / 60); 
    int lineFontSize = baseFontSize*(2);
    int buttonFontSize = baseFontSize;
    int comboBoxFontSize = baseFontSize;

    if(modo == CrearNuevoMapa){
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
        QFont labelFont("Tahoma", lineFontSize, QFont::Bold);
        tituloCantJugadores->setFont(labelFont);

        tituloCantJugadores->setStyleSheet(
            "QLabel {"
            "background-color: rgba(0, 0, 0, 0);"
            "color: yellow;"
            "padding: 25px 20px;"
            "font-size: 30px;"
            "}"
        );
    }

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

    comboBoxStyle = QString(
        "QComboBox {"
        "    color: #ffff66;"
        "    border: 1px solid #aaaaaa;"
        "    border-radius: 6px;"
        "    padding: 4px 8px;"
        "    font-size: %1px;"
        "    font-weight: bold;"
        "    min-width: 200px;"
        "    min-height: 40px;"
        "    background-color: #1e1e1e;"
        "}"

        "QComboBox:hover {"
        "    background-color: rgba(255, 255, 255, 0.05);"
        "    border: 1px solid yellow;"
        "    color: white;"
        "}"

        "QComboBox QAbstractItemView {"
        "    background-color: #2a2a2a;"
        "    selection-background-color: #ffaa00;"
        "    selection-color: black;"
        "    border: 1px solid #555555;"
        "    padding: 4px;"
        "    font-size: %1px;"
        "}"

        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 30px;"
        "    border-left: 1px solid #555555;"
        "}"
    ).arg(comboBoxFontSize);


}

void EditorWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    actualizarFondo();

    aplicarEstilosResponsivos();
}

void EditorWindow::actualizarFondo()
{
    QPixmap fondo("/var/taller/gfx/fondoSolo.jpg");
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
    
    QVBoxLayout *mainLayout = new QVBoxLayout(seleccionSpawnPoints);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(20);
    toolbarLayout->setAlignment(Qt::AlignCenter);

    opciones = new QComboBox();
    opciones->addItem("Elegir bloque:");
    opciones->addItem("Zona de Spawn");
    opciones->addItem("Zona de plantar bomba");
    toolbarLayout->addWidget(opciones);

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
    iconosScrollArea->setFixedHeight(50);  

    toolbarLayout->addWidget(iconosScrollArea);


    connect(opciones, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = opciones->currentText();

        limpiarIconosAnteriores(iconosLayoutSpawns, iconosActivosSpawns, iconMapperSpawns, iconoSeleccionadoSpawns);

        if (seleccion == "Zona de Spawn") {
            crearIconosSpawns();
        }
        else if (seleccion == "Zona de plantar bomba") {
            crearIconosZonaBomba();
        }
    });

    mainLayout->addLayout(toolbarLayout);

    QScrollArea* scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(false);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    scrollArea->setMaximumSize(3000, 3000); 
    scrollArea->setMinimumSize(400, 300); 
    
    QWidget* gridWidget = new QWidget();

    gridLayoutSpawns = new QGridLayout(gridWidget);
    gridLayoutSpawns->setSpacing(0);
    gridLayoutSpawns->setContentsMargins(0, 0, 0, 0);

    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

   
    int filas = 20;
    int columnas = 20;
    grillasCeldasSpawns.clear();
    grillasCeldasSpawns.resize(filas);

    
    matrizGrillaSpawns.resize(filas);
    for (int i = 0; i < filas; ++i) {
        matrizGrillaSpawns[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            matrizGrillaSpawns[i][j] = 0;
        }
    }

    for (int i = 0; i < filas; ++i) {
        grillasCeldasSpawns[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            QLabel* celda = new QLabel();

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

            celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            celda->setAcceptDrops(true);
            celda->setAttribute(Qt::WA_DeleteOnClose);

            gridLayoutSpawns->addWidget(celda, i, j);
            grillasCeldasSpawns[i][j] = celda;

            celda->installEventFilter(this);
            celda->setProperty("fila", i);
            celda->setProperty("columna", j);
        }
    }
    configurarCeldasParaDrops(grillasCeldasSpawns);

    int anchoTotal = columnas * 32;
    int altoTotal = filas * 32;
    gridWidget->setFixedSize(anchoTotal, altoTotal);

    scrollArea->setWidget(gridWidget);

    scrollArea->setAlignment(Qt::AlignCenter);

    QWidget* centroContenedor = new QWidget();
    QGridLayout* centroLayout = new QGridLayout(centroContenedor);
    centroLayout->addWidget(scrollArea, 0, 0, Qt::AlignCenter);
    centroLayout->setContentsMargins(0, 0, 0, 0);
    centroLayout->setSpacing(0);

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

    connect(anchoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int w){
        scrollArea->setFixedWidth(w);
    });
    connect(altoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int h){
        scrollArea->setFixedHeight(h);
    });


    QHBoxLayout* accionesLayout = new QHBoxLayout();

    guardarSpawnBtn = new QPushButton("Guardar cambios");
    guardarSpawnBtn->setEnabled(false);

    accionesLayout->addWidget(guardarSpawnBtn);
    mainLayout->addLayout(accionesLayout);

    agregarFilaBtn = new QPushButton("Agregar fila");
    agregarColumnaBtn = new QPushButton("Agregar columna");

    accionesLayout->addWidget(agregarFilaBtn);
    accionesLayout->addWidget(agregarColumnaBtn);

    connect(agregarFilaBtn, &QPushButton::clicked, this, &EditorWindow::agregarFila);
    connect(agregarColumnaBtn, &QPushButton::clicked, this, &EditorWindow::agregarColumna);

    eliminarFilaBtn = new QPushButton("Eliminar fila");
    eliminarColumnaBtn = new QPushButton("Eliminar columna");

    accionesLayout->addWidget(eliminarFilaBtn);
    accionesLayout->addWidget(eliminarColumnaBtn);

    connect(eliminarFilaBtn, &QPushButton::clicked, this, &EditorWindow::eliminarFila);
    connect(eliminarColumnaBtn, &QPushButton::clicked, this, &EditorWindow::eliminarColumna);

    mainLayout->addLayout(accionesLayout);

    seleccionSpawnPoints->setLayout(mainLayout);
    actualizarEstadoBotonesDimensiones();
}

void EditorWindow::inicializarEditorMapa(int filas, int columnas, bool esNuevoMapa) {
    miMenuBar = this->menuBar();

    fileMenu = miMenuBar->addMenu("Archivo");

    guardarAction = new QAction("Guardar", this);
    guardarSalirAction = new QAction("Guardar y Salir", this);

    fileMenu->addAction(guardarAction);
    fileMenu->addSeparator();
    fileMenu->addAction(guardarSalirAction);
    

    connect(guardarAction, &QAction::triggered, this, &EditorWindow::guardarMapa);
    connect(guardarSalirAction,  &QAction::triggered, this, [this]() {                   
        this->close();  // Cerrar la ventana después de guardar
    });
    
    QVBoxLayout *mainLayout = new QVBoxLayout(editorMapaWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(20);
    toolbarLayout->setAlignment(Qt::AlignCenter);

    categoriaCombo = new QComboBox();
    categoriaCombo->addItem("Elegir bloque:");
    categoriaCombo->addItem("Pisos");
    categoriaCombo->addItem("Muros");
    toolbarLayout->addWidget(categoriaCombo);

    
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
    iconosScrollArea->setFixedHeight(50);

    toolbarLayout->addWidget(iconosScrollArea);


    connect(categoriaCombo, &QComboBox::currentIndexChanged, this, [this]{
        QString seleccion = categoriaCombo->currentText();

        limpiarIconosAnteriores(iconosLayout, iconosActivos, iconMapper, iconoSeleccionado);

        if (seleccion == "Pisos") {
            crearIconosPisos();
        }
        else if (seleccion == "Muros") {
            crearIconosMuros();
        }
    });
    mainLayout->addLayout(toolbarLayout);

    
    QScrollArea* scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(false);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    scrollArea->setMaximumSize(3000, 3000);
    scrollArea->setMinimumSize(400, 300); 
    
    QWidget* gridWidget = new QWidget();

    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if(esNuevoMapa) {
        grillaCeldas.clear();
        grillaCeldas.resize(filas);

        for (int i = 0; i < filas; ++i) {
            grillaCeldas[i].resize(columnas);
            for (int j = 0; j < columnas; ++j) {
                QLabel* celda = new QLabel();

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

                celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                celda->setAcceptDrops(true);
                celda->setAttribute(Qt::WA_DeleteOnClose);

                gridLayout->addWidget(celda, i, j);
                grillaCeldas[i][j] = celda;

                celda->installEventFilter(this);
                celda->setProperty("fila", i);
                celda->setProperty("columna", j);
            }
        }
        int anchoTotal = columnas * 32;
        int altoTotal = filas * 32;
        gridWidget->setFixedSize(anchoTotal, altoTotal);
    } else {
        cargarMatrizDesdeYaml();
       
        if (!matrizGrilla.empty()) {
            int filasReales = matrizGrilla.size();
            int columnasReales = matrizGrilla[0].size();
            int anchoTotal = columnasReales * 32;
            int altoTotal = filasReales * 32;
            gridWidget->setFixedSize(anchoTotal, altoTotal);
        }
    }

    configurarCeldasParaDrops(grillaCeldas);

    scrollArea->setWidget(gridWidget);

    scrollArea->setAlignment(Qt::AlignCenter);

    QWidget* centroContenedor = new QWidget();
    QGridLayout* centroLayout = new QGridLayout(centroContenedor);
    centroLayout->addWidget(scrollArea, 0, 0, Qt::AlignCenter);
    centroLayout->setContentsMargins(0, 0, 0, 0);
    centroLayout->setSpacing(0);

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

    connect(anchoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int w){
        scrollArea->setFixedWidth(w);
    });
    connect(altoSpin, QOverload<int>::of(&QSpinBox::valueChanged), scrollArea, [=](int h){
        scrollArea->setFixedHeight(h);
    });

    if(esNuevoMapa){
        editorMapaWidget->setLayout(mainLayout);
    }else{
        editorMapaExistenteWidget->setLayout(mainLayout);
    }

    
}

void EditorWindow::configurarCeldasParaDrops(QVector<QVector<QLabel*>>& grilla) {
    
    int numeroFilas = grilla.size();
    int numeroColumnas = grilla[0].size();
    for (int fila = 0; fila < numeroFilas; ++fila) {
        for (int columna = 0; columna < numeroColumnas; ++columna) {
            QLabel* celda = grilla[fila][columna];
            celda->setAcceptDrops(true);
        }
    }
    for (const auto& fila : grilla) {
        for (QLabel* celda : fila) {
            if (celda) {
                celda->setAcceptDrops(true);
            }
        }
    }
}

bool EditorWindow::eventSpawns(QObject* obj, QEvent* event)
{
    QLabel* celda = qobject_cast<QLabel*>(obj);
    if (!celda) {
        return QMainWindow::eventFilter(obj, event);
    }

    manejoDragAndDrop(obj, event);

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QLabel* celda = qobject_cast<QLabel*>(obj);
        if (!celda) {
            return QMainWindow::eventFilter(obj, event);
        }
        if (mouseEvent->button() == Qt::RightButton) {
            return alClickearDerechoSpawns(celda);
        }
        else if (mouseEvent->button() == Qt::LeftButton && 
                (!pixmapSeleccionadoSpawns.isNull() || !bloqueSeleccionadoSpawns.isEmpty())) {
            return alClickearIzquierdoSpawns(celda);
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

bool EditorWindow::alClickearIzquierdoSpawns(QLabel*& celda)
{
    int fila = celda->property("fila").toInt();
    int columna = celda->property("columna").toInt();

   celda->clear();

    if (!bloqueSeleccionadoSpawns.isEmpty()) {
        QPixmap pixmap(bloqueSeleccionadoSpawns);
        if (!pixmap.isNull()) {
            celda->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else if (!pixmapSeleccionadoSpawns.isNull()) { 
        if(coordenadasSeleccionadasSpawns.first == 4 && coordenadasSeleccionadasSpawns.second == 7) {
            matrizGrillaSpawns[fila][columna] = 3; // Zona de spawnA
            ++terroristas;
            verificarCondicionFinal();
        }
        else if(coordenadasSeleccionadasSpawns.first == 9 && coordenadasSeleccionadasSpawns.second == 0) {
            matrizGrillaSpawns[fila][columna] = 4; // Zona de spawnB
            ++antiterroristas;
            verificarCondicionFinal();  
        }
        else if(coordenadasSeleccionadasSpawns.first == 0 && coordenadasSeleccionadasSpawns.second == 0) {
            matrizGrillaSpawns[fila][columna] = 2; // Zona de plantar bomba
            ++cantZonaPlantable;
            verificarCondicionFinal(); 
        }
        
        celda->setPixmap(pixmapSeleccionadoSpawns.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        qDebug() << "Bloque colocado en (" << fila << "," << columna 
                << ") con coordenadas del tileset (" 
                << coordenadasSeleccionadasSpawns.first << "," 
                << coordenadasSeleccionadasSpawns.second << ")";
    }

    celda->setAlignment(Qt::AlignCenter);
    return true;
}
bool EditorWindow::alClickearDerechoSpawns(QLabel*& celda)
{
    int fila = celda->property("fila").toInt();
    int columna = celda->property("columna").toInt();

    celda->clear();
    celda->setPixmap(QPixmap()); 
    
    celda->setStyleSheet(
        "background-color: #f0f0f0; "
        "border: 0.5px solid #999999; "
        "margin: 0px; "
        "padding: 0px;"
    );
    celda->setAlignment(Qt::AlignCenter);
    
    int valorAnterior = matrizGrillaSpawns[fila][columna];
    if (valorAnterior == 3) {
        --terroristas;
        verificarCondicionFinal(); 
    } else if (valorAnterior == 4) {
        --antiterroristas;
        verificarCondicionFinal();  

    } else if (valorAnterior == 2) {
        --cantZonaPlantable;
        verificarCondicionFinal(); 

    }
    
    matrizGrillaSpawns[fila][columna] = 0; 
    
    qDebug() << "Celda restaurada en (" << fila << "," << columna << ")";
    return true;
}

bool EditorWindow::eventEditar(QObject* obj, QEvent* event)
{
    if (manejoDragAndDrop(obj, event)) {
        return true;
    }
    
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QLabel* celda = qobject_cast<QLabel*>(obj);
        if (!celda) {
            return QMainWindow::eventFilter(obj, event);
        }
        if (mouseEvent->button() == Qt::RightButton) {
            return alClickearDerecho(celda);
        }
        else if (mouseEvent->button() == Qt::LeftButton && 
                (!pixmapSeleccionado.isNull() || !bloqueSeleccionado.isEmpty())) {
            return alClickearIzquierdo(celda);
        }
    }

    return QMainWindow::eventFilter(obj, event);
}


bool EditorWindow::alClickearIzquierdo(QLabel*& celda)
{
    int fila = celda->property("fila").toInt();
    int columna = celda->property("columna").toInt();

    celda->clear();

    if (!bloqueSeleccionado.isEmpty()) {
        QPixmap pixmap(bloqueSeleccionado);
        if (!pixmap.isNull()) {
            celda->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else if (!pixmapSeleccionado.isNull()) {
        if((matrizGrillaSpawns[fila][columna] == 3 || matrizGrillaSpawns[fila][columna] == 4 || matrizGrillaSpawns[fila][columna] == 2) && solid_blocks.contains(coordenadasSeleccionadas)) {
            QMessageBox::warning(this,                      
                             "Zona de spawn/plantado de bomba",     
                             "No podés colocar un bloque tipo muro en este casillero."); 
            alClickearDerecho(celda); 
        } else {
            matrizGrilla[fila][columna] = coordenadasSeleccionadas;
            
            celda->setPixmap(pixmapSeleccionado.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            
            qDebug() << "Bloque colocado en (" << fila << "," << columna 
                    << ") con coordenadas del tileset (" 
                    << coordenadasSeleccionadas.first << "," 
                    << coordenadasSeleccionadas.second << ")";
        }
        
    }

    celda->setAlignment(Qt::AlignCenter);
    return true;
}

bool EditorWindow::alClickearDerecho(QLabel*& celda)
{
    int fila = celda->property("fila").toInt();
    int columna = celda->property("columna").toInt();

    celda->clear();
    celda->setPixmap(QPixmap());
    
    celda->setStyleSheet(
        "background-color: #f0f0f0; "
        "border: 0.5px solid #999999; "
        "margin: 0px; "
        "padding: 0px;"
    );
    celda->setAlignment(Qt::AlignCenter);
    
    matrizGrilla[fila][columna] = std::make_pair(0, 0); // o el valor que uses como base
    
    qDebug() << "Celda restaurada en (" << fila << "," << columna << ")";
    return true;
}

bool EditorWindow::manejoDragAndDrop(QObject *obj, QEvent *event)
{
    QLabel* celda = qobject_cast<QLabel*>(obj);
    if (event->type() == QEvent::DragEnter) {
        QDragEnterEvent* dragEvent = static_cast<QDragEnterEvent*>(event);
        if (dragEvent->mimeData()->hasFormat("application/x-tile-coords")) {
            dragEvent->acceptProposedAction();
            QString estiloOriginal = celda->styleSheet();
            if (estiloOriginal.contains("background-color: #f0f0f0")) {
                celda->setStyleSheet(estiloOriginal.replace("#f0f0f0", "#e0e0ff"));
            }
            return true;
        }
        return false;
    }
    
    if (event->type() == QEvent::DragLeave) {
        if (celda->pixmap().isNull()) {
            celda->setStyleSheet(
                "background-color: #f0f0f0; "
                "border: 0.5px solid #999999; "
                "margin: 0px; "
                "padding: 0px;"
            );
        }
        return true;
    }
    
    if (event->type() == QEvent::Drop) {
        QDropEvent* dropEvent = static_cast<QDropEvent*>(event);
        if (dropEvent->mimeData()->hasFormat("application/x-tile-coords")) {
            QByteArray itemData = dropEvent->mimeData()->data("application/x-tile-coords");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);
            int dragFila, dragColumna;
            dataStream >> dragFila >> dragColumna;
            
            int fila = celda->property("fila").toInt();
            int columna = celda->property("columna").toInt();
            
            QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");
            int tileWidth = 32;
            int tileHeight = 32;
            int x = dragColumna * tileWidth;
            int y = dragFila * tileHeight;
            QPixmap croppedPixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);
            
            celda->clear();
            celda->setPixmap(croppedPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            celda->setAlignment(Qt::AlignCenter);
            
            if (stackedWidget->currentWidget() == seleccionSpawnPoints) {
                if(dragFila == 4 && dragColumna == 7) {
                    matrizGrillaSpawns[fila][columna] = 3; // Zona de spawn terrorista
                    ++terroristas;
                    verificarCondicionFinal(); 
                }
                else if(dragFila == 9 && dragColumna == 0) {
                    matrizGrillaSpawns[fila][columna] = 4; // Zona de spawn antiterrorista
                    ++antiterroristas;
                    verificarCondicionFinal();
                }
                else if(dragFila == 0 && dragColumna == 0) {
                    matrizGrillaSpawns[fila][columna] = 2; // Zona de plantar bomba
                    ++cantZonaPlantable;
                    verificarCondicionFinal(); 
                } else {
                    matrizGrillaSpawns[fila][columna] = 1;
                }
            } else {
                QPair<int, int> coord(dragFila, dragColumna);

                if ((matrizGrillaSpawns[fila][columna] == 3 || 
                    matrizGrillaSpawns[fila][columna] == 4 || 
                    matrizGrillaSpawns[fila][columna] == 2) && 
                    solid_blocks.contains(coord)) {
                    
                    QMessageBox::warning(this,                      
                             "Zona de spawn/plantado de bomba",    
                             "No podés colocar un bloque tipo muro en este casillero."); 
                    alClickearDerecho(celda); 
                } else {
                    matrizGrilla[fila][columna] = {dragFila, dragColumna};
                }
            
            }
            
            qDebug() << "Tile aplicado por drag en (" << fila << "," << columna 
                     << ") desde tileset (" << dragFila << "," << dragColumna << ")";
            
            dropEvent->acceptProposedAction();
            return true;
        }
        return false;
    }
    return false;
}


bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if(stackedWidget->currentWidget() == menuInicialWidget){
        if(modo == CrearNuevoMapa) {
            if (obj == nombre_mapa && event->type() == QEvent::FocusIn) {
                if (nombre_mapa->text() == "Nombre del mapa") {
                    nombre_mapa->clear();
                }
            }
            if (obj == nombre_mapa && event->type() == QEvent::FocusOut) {
                if (nombre_mapa->text().isEmpty()) {
                    nombre_mapa->setText("Nombre del mapa");
                }
            }
            if (obj == cant_jugadores && event->type() == QEvent::FocusIn) {
                if (nombre_mapa->text() == "Nombre del mapa") {
                    nombre_mapa->clear();
                }
            }
            if (obj == cant_jugadores && event->type() == QEvent::FocusOut) {
                if (nombre_mapa->text().isEmpty()) {
                    nombre_mapa->setText("Nombre del mapa");
                }
            }
        }else{

        }
        return QMainWindow::eventFilter(obj, event);
    }else{
        if (stackedWidget->currentWidget() == seleccionSpawnPoints) {
            return eventSpawns(obj, event);
            
        } else {
            return eventEditar(obj, event);
        }
    }
}


void EditorWindow::actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado, 
                                             QList<ClickableLabel*>& iconsActivs, ClickableLabel*& iconSelec) {
    if (!nuevoSeleccionado) return;

    if (iconSelec && iconsActivs.contains(iconSelec)) {
        iconSelec->setStyleSheet("border: 1px solid #999999;");
    }

    nuevoSeleccionado->setStyleSheet("border: 2px solid yellow; background-color: rgba(255, 255, 0, 0.2);");
    iconSelec = nuevoSeleccionado;
}

void EditorWindow::actualizarTamanoGridWidget() {
    if (grillasCeldasSpawns.isEmpty()) return;

    int filas = grillasCeldasSpawns.size();
    int columnas = grillasCeldasSpawns[0].size();
    int anchoTotal = columnas * 32;
    int altoTotal = filas * 32;

    QWidget* gridWidget = gridLayoutSpawns->parentWidget();
    if (gridWidget) {
        gridWidget->setFixedSize(anchoTotal, altoTotal);
    }
}
 

void EditorWindow::verificarCondicionFinal() {
    qDebug() << "Verificando condición final:"
             << terroristas << antiterroristas << cantZonaPlantable << jugadoresMaximos;

    if (terroristas == jugadoresMaximos && 
        antiterroristas == jugadoresMaximos && 
        cantZonaPlantable>=4) {
        guardarSpawnBtn->setEnabled(true);
        qDebug() << "Condición final cumplida.";
    } else {
        guardarSpawnBtn->setEnabled(false);
    }
}




void EditorWindow::limpiarIconosAnteriores(QHBoxLayout*& iconosLay, QList<ClickableLabel*>& iconosActivs, QSignalMapper*& iconMapperSs, ClickableLabel*& iconoSelec) {
    for (ClickableLabel* icono : iconosActivs) {
        if (icono) {
            disconnect(icono, nullptr, this, nullptr);
        }
    }
    iconosActivs.clear();

    QLayoutItem* child;
    while ((child = iconosLay->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    if (iconMapperSs) {
        disconnect(iconMapperSs, nullptr, this, nullptr);
        iconMapperSs->deleteLater();
        iconMapperSs = nullptr;
    }

    iconoSelec = nullptr;
}

void EditorWindow::crearIconosPisos() {
    iconMapper = new QSignalMapper(this);

    QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");

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
            icono->setDragData(croppedPixmap, fila, columna);
            icono->setDraggable(true);

            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                coordenadasSeleccionadas = {fila, columna}; 
                actualizarSeleccionVisual(icono, iconosActivos, iconoSeleccionado);
            });
        }
    }
}

void EditorWindow::crearIconosMuros() {
    iconMapper = new QSignalMapper(this);

    QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {2,4},{2,5},{2,6},{2,7},{9,2},{9,3},{9,4},  // cajas
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
            icono->setDragData(croppedPixmap, fila, columna);
            icono->setDraggable(true);

            iconosActivos.append(icono);
            iconosLayout->addWidget(icono);

            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionado = croppedPixmap;
                coordenadasSeleccionadas = {fila, columna}; // NUEVA LÍNEA
                actualizarSeleccionVisual(icono, iconosActivos, iconoSeleccionado);
            });
        }
    }
}

void EditorWindow::crearIconosSpawns() {

    iconMapperSpawns = new QSignalMapper(this);

    QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> bloquesValidos = {
        {9,0},{4,7}
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
            icono->setDragData(croppedPixmap, fila, columna);
            icono->setDraggable(true);

            iconosActivosSpawns.append(icono);
            iconosLayoutSpawns->addWidget(icono);

            connect(icono, &ClickableLabel::clicked, this, [this, croppedPixmap, icono, fila, columna]() {
                bloqueSeleccionado = "";
                pixmapSeleccionadoSpawns = croppedPixmap;
                coordenadasSeleccionadasSpawns = {fila, columna};
                actualizarSeleccionVisual(icono, iconosActivosSpawns, iconoSeleccionadoSpawns);
            });
        }
    }
}


void EditorWindow::crearIconosZonaBomba() {
    iconMapperSpawns = new QSignalMapper(this);

    QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");

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
            icono->setDragData(croppedPixmap, fila, columna);
            icono->setDraggable(true);

            iconosActivosSpawns.append(icono);
            iconosLayoutSpawns->addWidget(icono);

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


void EditorWindow::agregarFila() {
    int nuevasFilas = grillasCeldasSpawns.size() + 1;
    int columnas = grillasCeldasSpawns.isEmpty() ? 0 : grillasCeldasSpawns[0].size();

    grillasCeldasSpawns.resize(nuevasFilas);
    grillasCeldasSpawns[nuevasFilas - 1].resize(columnas);


    matrizGrillaSpawns.resize(nuevasFilas);
    matrizGrillaSpawns[nuevasFilas - 1].resize(columnas, 0);

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
        celda->setAcceptDrops(true);
        celda->setAttribute(Qt::WA_DeleteOnClose);
        celda->installEventFilter(this);
        celda->setProperty("fila", nuevasFilas - 1);
        celda->setProperty("columna", j);

        if (!gridLayoutSpawns) return;
        gridLayoutSpawns->addWidget(celda, nuevasFilas - 1, j);
        grillasCeldasSpawns[nuevasFilas - 1][j] = celda;
    }

    qDebug() << grillasCeldasSpawns.size();

    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
}

void EditorWindow::agregarColumna() {
    int filas = grillasCeldasSpawns.size();
    if (filas == 0) return;

    int nuevasColumnas = grillasCeldasSpawns[0].size() + 1;

    for (int i = 0; i < filas; ++i) {
        grillasCeldasSpawns[i].resize(nuevasColumnas);

        matrizGrillaSpawns[i].resize(nuevasColumnas, 0);

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
        celda->setAcceptDrops(true);
        celda->setAttribute(Qt::WA_DeleteOnClose);
        celda->installEventFilter(this);
        celda->setProperty("fila", i);
        celda->setProperty("columna", nuevasColumnas - 1);

        if (!gridLayoutSpawns) return;
        gridLayoutSpawns->addWidget(celda, i, nuevasColumnas - 1);
        grillasCeldasSpawns[i][nuevasColumnas - 1] = celda;
    }

    qDebug() << grillasCeldasSpawns[0].size();

    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    
}

void EditorWindow::eliminarFila() {
    int filas = grillasCeldasSpawns.size();
    if (filas <= 1) return;

    if (!gridLayoutSpawns) return;
    int columnas = grillasCeldasSpawns[0].size();
    for (int j = 0; j < columnas; ++j) {
        QLabel* celda = grillasCeldasSpawns[filas - 1][j];
        gridLayoutSpawns->removeWidget(celda);
        delete celda;
    }

    grillasCeldasSpawns.removeLast();
    matrizGrillaSpawns.removeLast();

    qDebug() << grillasCeldasSpawns.size();

    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    
    
}

void EditorWindow::eliminarColumna() {
    if (grillasCeldasSpawns.isEmpty() || grillasCeldasSpawns[0].size() <= 1) return;

    if (!gridLayoutSpawns) return;
    int ultimaColumna = grillasCeldasSpawns[0].size() - 1;
    for (int i = 0; i < grillasCeldasSpawns.size(); ++i) {
        QLabel* celda = grillasCeldasSpawns[i][ultimaColumna];
        gridLayoutSpawns->removeWidget(celda);
        delete celda;
        grillasCeldasSpawns[i].removeLast();
        matrizGrillaSpawns[i].removeLast();
    }

    
    qDebug() << grillasCeldasSpawns[0].size();
    actualizarTamanoGridWidget();
    actualizarEstadoBotonesDimensiones();
    
    
}

void EditorWindow::actualizarEstadoBotonesDimensiones() {
    int filas = grillasCeldasSpawns.size();
    int columnas = grillasCeldasSpawns[0].size();

    agregarFilaBtn->setEnabled(filas < MAX_FILAS);
    eliminarFilaBtn->setEnabled(filas > MIN_FILAS);

    agregarColumnaBtn->setEnabled(columnas < MAX_COLUMNAS);
    eliminarColumnaBtn->setEnabled(columnas > MIN_COLUMNAS);
}

MapData EditorWindow::crearMapData() {
    MapData mapData;
    std::string background_path;
    std::string sprite_path;

    int filas = grillaCeldas.size();
    int columnas = grillaCeldas.isEmpty() ? 0 : grillaCeldas[0].size();

    std::vector<std::vector<CellType>> game_map = std::vector<std::vector<CellType>>(filas, std::vector<CellType>(columnas));

    std::vector<std::vector<uint16_t>> tiles_map = std::vector<std::vector<uint16_t>>(filas, std::vector<uint16_t>(columnas));
    std::unordered_map<uint16_t, MapLegendEntry> legend_tiles;

    int hor_tile_amount = tiles.width() / 32;
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            int numeric_value = matrizGrilla[i][j].first % hor_tile_amount +  matrizGrilla[i][j].second * hor_tile_amount;
            tiles_map[i][j] = numeric_value;
            if (!legend_tiles.contains(numeric_value)) {
                legend_tiles[numeric_value] = MapLegendEntry{matrizGrilla[i][j].first * 32, matrizGrilla[i][j].second * 32};
            }
            if (solid_blocks.contains(matrizGrilla[i][j])) {
                game_map[i][j] = CellType::Blocked;
            } else {
                game_map[i][j] = CellType::Walkable;
            }
            game_map[i][j] = static_cast<CellType>(matrizGrillaSpawns[i][j]);
        }
    }
    mapData.background_path = "/var/taller/gfx/backgrounds/dust.png";
    mapData.sprite_path = "/var/taller/gfx/tiles/dust.bmp";
    mapData.game_map = game_map;
    mapData.tiles_map = tiles_map;
    mapData.legend_tiles = legend_tiles;

    return mapData;
}


void EditorWindow::crearArchivoYamlInicial() {
    
    QString directorioMaps = "/var/taller/maps";
    
    QString nombreMapa = nombre_mapa->text();
    QString nombreArchivo = nombreMapa;
    nombreArchivo.replace(" ", "_");
    QString baseNombreArchivo = nombreArchivo;
    QString extension = ".yaml";
    QString rutaArchivo = QDir::cleanPath(directorioMaps + "/" + nombreArchivo + extension);

    int contador = 1;
    while (QFile::exists(rutaArchivo)) {
        nombreArchivo = baseNombreArchivo + "_" + QString::number(contador);
        rutaArchivo = QDir::cleanPath(directorioMaps + "/" + nombreArchivo + extension);
        ++contador;
    }

    rutaArchivoActual = rutaArchivo;

    
    
    QFileInfo dirInfo(directorioMaps);
    if (!dirInfo.isWritable()) {
        QMessageBox::StandardButton respuesta = QMessageBox::question(this, 
            "Permisos requeridos",
            "El directorio /var/taller/maps no tiene permisos de escritura.\n\n"
            "¿Deseas que la aplicación intente configurar los permisos?\n"
            "(Se requerirá contraseña de administrador)",
            QMessageBox::Yes | QMessageBox::No);
        
        if (respuesta == QMessageBox::Yes) {
            QString comando = QString("pkexec chown %1:%1 /var/taller/maps && pkexec chmod 775 /var/taller/maps")
                            .arg(qgetenv("USER"));
            
            QProcess proceso;
            proceso.start("sh", QStringList() << "-c" << comando);
            proceso.waitForFinished();
            
            if (proceso.exitCode() == 0) {
                QMessageBox::information(this, "Éxito", "Permisos configurados correctamente.");
                dirInfo.refresh();
            } else {
                QMessageBox::critical(this, "Error", 
                    "No se pudieron configurar los permisos automáticamente.\n\n"
                    "Ejecuta manualmente en terminal:\n"
                    "sudo chown $USER:$USER /var/taller/maps\n"
                    "chmod 775 /var/taller/maps");
                return;
            }
        } else {
            QMessageBox::information(this, "Información", 
                "Para usar esta función, configura los permisos manualmente:\n\n"
                "sudo chown $USER:$USER /var/taller/maps\n"
                "chmod 775 /var/taller/maps");
            return;
        }
    }
    
    rutaArchivoActual = QDir::cleanPath(directorioMaps + "/" + nombreArchivo + extension);
    
    QFile archivo(rutaArchivoActual);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", 
            "No se pudo crear el archivo YAML: " + nombreArchivo + 
            "\nError: " + archivo.errorString());
        return;
    }
    
    QTextStream stream(&archivo);
    
    int filas = matrizGrillaSpawns.size();
    int columnas = matrizGrillaSpawns[0].size();
    matrizGrilla.resize(filas);
    for (int i = 0; i < filas; ++i) {
        matrizGrilla[i].resize(columnas);
        for (int j = 0; j < columnas; ++j) {
            matrizGrilla[i][j] = {0, 0};
        }
    }
    
    archivo.close();
    
    QMessageBox::information(this, "Archivo Creado",
        "Archivo YAML creado: " + nombreArchivo + "\n" +
        "Ubicación: " + rutaArchivoActual);
    qDebug() << "Archivo YAML inicial creado en:" << rutaArchivoActual;
}



void EditorWindow::diagnosticarDirectorio() {
    QString directorioMaps = "/var/taller/maps";
    
    qDebug() << "=== DIAGNÓSTICO DEL DIRECTORIO ===";
    qDebug() << "Ruta a verificar:" << directorioMaps;
    
    QDir dir(directorioMaps);
    qDebug() << "¿Existe el directorio?" << dir.exists();
    
    QFileInfo dirInfo(directorioMaps);
    qDebug() << "¿Es directorio?" << dirInfo.isDir();
    qDebug() << "¿Es legible?" << dirInfo.isReadable();
    qDebug() << "¿Es escribible?" << dirInfo.isWritable();
    qDebug() << "¿Es ejecutable?" << dirInfo.isExecutable();
    qDebug() << "Propietario:" << dirInfo.owner();
    qDebug() << "Grupo:" << dirInfo.group();
    qDebug() << "Permisos:" << QString::number(dirInfo.permissions(), 8);
    
    QFileInfo parentInfo("/var/taller");
    qDebug() << "¿Existe /var/taller?" << parentInfo.exists();
    qDebug() << "¿Es escribible /var/taller?" << parentInfo.isWritable();
}

void EditorWindow::guardarProgresoEnYaml() {
    if (rutaArchivoActual.isEmpty()) {
        QMessageBox::critical(this, "Error", "No hay un archivo activo para guardar.");
        return;
    }
    
    QFileInfo archivoInfo(rutaArchivoActual);
    QString directorio = archivoInfo.absolutePath();
    
    QFileInfo dirInfo(directorio);
    if (!dirInfo.exists()) {
        QMessageBox::critical(this, "Error", 
            "El directorio no existe: " + directorio);
        return;
    }
    
    if (!dirInfo.isWritable()) {
        QMessageBox::StandardButton respuesta = QMessageBox::question(this, 
            "Permisos requeridos",
            "El directorio no tiene permisos de escritura: " + directorio + "\n\n"
            "¿Deseas que la aplicación intente configurar los permisos?\n"
            "(Se requerirá contraseña de administrador)",
            QMessageBox::Yes | QMessageBox::No);
        
        if (respuesta == QMessageBox::Yes) {
            QString comando = QString("pkexec chown %1:%1 %2 && pkexec chmod 775 %2")
                            .arg(qgetenv("USER"))
                            .arg(directorio);
            
            QProcess proceso;
            proceso.start("sh", QStringList() << "-c" << comando);
            proceso.waitForFinished();
            
            if (proceso.exitCode() != 0) {
                QMessageBox::critical(this, "Error", 
                    "No se pudieron configurar los permisos automáticamente.\n\n"
                    "Ejecuta manualmente en terminal:\n"
                    "sudo chown $USER:$USER " + directorio + "\n"
                    "chmod 775 " + directorio);
                return;
            }
            
            dirInfo.refresh();
        } else {
            QMessageBox::information(this, "Información", 
                "Para guardar, configura los permisos manualmente:\n\n"
                "sudo chown $USER:$USER " + directorio + "\n"
                "chmod 775 " + directorio);
            return;
        }
    }
    
    qDebug() << "Intentando guardar archivo en:" << rutaArchivoActual;
    
    QFile archivo(rutaArchivoActual);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString errorDetallado = QString("No se pudo abrir el archivo YAML para guardar:\n"
                                       "Archivo: %1\n"
                                       "Error del sistema: %2\n"
                                       "Código de error: %3")
                                .arg(rutaArchivoActual)
                                .arg(archivo.errorString())
                                .arg(archivo.error());
        
        QMessageBox::critical(this, "Error", errorDetallado);
        return;
    }
    
    QTextStream stream(&archivo);
    
    if (matrizGrilla.empty() || matrizGrilla[0].empty()) {
        archivo.close();
        QMessageBox::critical(this, "Error", "No hay datos válidos para guardar.");
        return;
    }
    
    int filas = matrizGrilla.size();
    int columnas = matrizGrilla[0].size();
    
    try {
        
        MapData mapData = crearMapData();
        
        stream << "map:" << "\n";
        stream << "  name: " << nombreArchivoActual << "\n";
        stream << "  width: " << columnas << "\n";
        stream << "  height: " << filas << "\n";
        stream << "  background_path: " << QString::fromStdString(mapData.background_path) << "\n";
        stream << "  sprite_path: " << QString::fromStdString(mapData.sprite_path) << "\n";
        stream << "\n";
        
        stream << "  tiles:" << "\n";
        

        for (int i = 0; i < filas; ++i) {
            if (i != 0) {
                stream << "\n";
            }
            stream << "  - ["; 
            for (int j = 0; j < columnas; ++j) {
                stream << mapData.tiles_map[i][j];
                if (j < columnas - 1) {
                    stream << ","; 
                }
            }
            if (i < filas) {
                stream << "]"; 
            }
        }
        stream << "\n\n";
        
        stream << "  game:" << "\n";
        for (int i = 0; i < filas; ++i) {
            if (i != 0) {
                stream << "\n";
            }
            stream << "  - ["; 
            for (int j = 0; j < columnas; ++j) {
                stream << static_cast<int>(mapData.game_map[i][j]);
                if (j < columnas - 1) {
                    stream << ","; 
                }
            }
            if (i < filas) {
                stream << "]";
            }
        }
        stream << "\n\n";
        
        stream << "legend_tiles:" << "\n";
        for (auto [num, entry] : mapData.legend_tiles) {
            stream << "  " << num << ":" << "\n";
            stream << "    x: " << entry.x << "\n";
            stream << "    y: " << entry.y << "\n";
        }
        
        if (stream.status() != QTextStream::Ok) {
            archivo.close();
            QMessageBox::critical(this, "Error", "Error al escribir datos en el archivo.");
            return;
        }
        
    } catch (const std::exception& e) {
        archivo.close();
        QMessageBox::critical(this, "Error", 
            QString("Error al procesar los datos: %1").arg(e.what()));
        return;
    }
    
    archivo.close();
    
    QFileInfo savedFileInfo(rutaArchivoActual);
    if (!savedFileInfo.exists() || savedFileInfo.size() == 0) {
        QMessageBox::critical(this, "Error", 
            "El archivo parece no haberse guardado correctamente.");
        return;
    }
    
    QMessageBox::information(this, "Guardado Exitoso",
        "Mapa guardado correctamente en: " + rutaArchivoActual + 
        "\nTamaño del archivo: " + QString::number(savedFileInfo.size()) + " bytes");
    qDebug() << "Progreso guardado exitosamente en:" << rutaArchivoActual;
}


void EditorWindow::onCrearMapaClicked() {

    QString name = nombre_mapa->text();

    if (name.isEmpty() || name == "Nombre del mapa") {
        QMessageBox::warning(this, "Error", "Por favor ingresa un nombre válido para el mapa");
        return;
    }

    QString textoCantJugadores = cant_jugadores->text();
    jugadoresMaximos = textoCantJugadores.toInt();

    terroristas = 0;
    antiterroristas = 0;
    cantZonaPlantable = 0;

    inicializarSpawnsMapa();


    connect(guardarSpawnBtn, &QPushButton::clicked, this, &EditorWindow::onGuardarSpawnMapa);
    
    stackedWidget->setCurrentIndex(1);
}

void EditorWindow::onEditarMapaClicked() {
    rutaArchivoActual = mapasExistentes->currentData().toString();

    if (rutaArchivoActual.isEmpty() || rutaArchivoActual == "Elegir mapa:") {
        QMessageBox::warning(this, "Error", "Por favor ingresa un nombre válido para el mapa");
        return;
    }
        
    nombreArchivoActual = mapasExistentes->currentText();
    const std::string& ruta = rutaArchivoActual.toStdString();

    inicializarEditorMapa(0,0,false);
    stackedWidget->setCurrentIndex(3);

    QMessageBox::information(this, "Mapa Cargado",
                             "Mapa cargado: " + nombreArchivoActual);
}

void EditorWindow::onSalirClicked()
{
    this->close();
}

void EditorWindow::onGuardarSpawnMapa()
{
    crearArchivoYamlInicial();
    
    int filas = grillasCeldasSpawns.size();
    int columnas = grillasCeldasSpawns.isEmpty() ? 0 : grillasCeldasSpawns[0].size();

    inicializarEditorMapa(filas,columnas,true);
    stackedWidget->setCurrentIndex(2);

}

EditorWindow::~EditorWindow()
{

}


void EditorWindow::guardarMapa()
{
    if (rutaArchivoActual.isEmpty()) {
        QMessageBox::warning(this, "Error", "No hay archivo YAML asociado a este mapa");
        return;
    }

    guardarProgresoEnYaml();
}


void EditorWindow::closeEvent(QCloseEvent* event) {

    if (stackedWidget->currentWidget() == seleccionSpawnPoints) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmar salida",
                                    "¿Estás seguro de que querés cerrar la ventana? \n Se borrara el progreso actual.",
                                    QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }

    }
    else if (stackedWidget->currentWidget() == menuInicialWidget) {
        event->accept();
    }else if(stackedWidget->currentWidget() == editorMapaExistenteWidget){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmar salida",
                                    "¿Desea guardar los cambios? \n Se borrara el progreso actual si no guardo previamente.",
                                    QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            guardarProgresoEnYaml();  
            event->accept();
        } else {
            event->accept();
        }
    }
    else{
        guardarProgresoEnYaml();  
        event->accept();
    }
   
}



void EditorWindow::cargarArchivosYamlEnComboBox(QComboBox* comboBox) {
    QDir dir("/var/taller/maps");

    QStringList filtros;
    filtros << "*.yaml";
    dir.setNameFilters(filtros);

    QStringList archivos = dir.entryList(QDir::Files);

    comboBox->clear();
    comboBox->addItem("Elegir mapa:");
    mapasExistentes->setStyleSheet(comboBoxStyle);

    for (const QString& archivo : archivos) {
        comboBox->addItem(archivo, dir.absoluteFilePath(archivo));
    }
}


void EditorWindow::limpiarGrillaExistente() {
    if (!grillaCeldas.isEmpty()) {
        for (int i = 0; i < grillaCeldas.size(); ++i) {
            for (int j = 0; j < grillaCeldas[i].size(); ++j) {
                if (grillaCeldas[i][j]) {
                    gridLayout->removeWidget(grillaCeldas[i][j]);
                    delete grillaCeldas[i][j];
                    grillaCeldas[i][j] = nullptr;
                }
            }
        }
        grillaCeldas.clear();
    }
}

void EditorWindow::crearGrillaVisual(int filas, int columnas) {
    qDebug() << "Creando grilla visual:" << filas << "x" << columnas;
    
    grillaCeldas.resize(filas);
    for (int i = 0; i < filas; ++i) {
        grillaCeldas[i].resize(columnas);
    }
    
    
    for (int fila = 0; fila < filas; ++fila) {
        for (int columna = 0; columna < columnas; ++columna) {
            QLabel* celda = new QLabel();
            
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
            celda->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            celda->setAcceptDrops(true);
            celda->setAttribute(Qt::WA_DeleteOnClose);
            
            celda->setProperty("fila", fila);
            celda->setProperty("columna", columna);
            
            celda->installEventFilter(this);
            
            gridLayout->addWidget(celda, fila, columna);
            grillaCeldas[fila][columna] = celda;
        }
    }
    
    configurarCeldasParaDrops(grillaCeldas);
}

void EditorWindow::cargarMatrizDesdeYaml()
{
    const std::string& ruta = rutaArchivoActual.toStdString();
    qDebug() << "Cargando matriz desde YAML:" << QString::fromStdString(ruta);
    
    try {
        Map mapa(ruta);
        std::vector<std::vector<uint16_t>> tiles_map = mapa.get_tiles_map();
        std::vector<std::vector<CellType>> game_map = mapa.get_game_map();
        
        matrizGrilla.clear();
        matrizGrillaSpawns.clear();
        
        int filas = tiles_map.size();
        int columnas = tiles_map[0].size();
        
        qDebug() << "Dimensiones del mapa cargado: " << filas << "x" << columnas;
        
        limpiarGrillaExistente();
        
        matrizGrilla.resize(filas);
        matrizGrillaSpawns.resize(filas);

        for (int i = 0; i < filas; ++i) {
            matrizGrilla[i].resize(columnas);
            matrizGrillaSpawns[i].resize(columnas);
            for (int j = 0; j < columnas; ++j) {
                
                int fila = mapa.get_tiles_legend(tiles_map[i][j]).x / 32;
                int columna = mapa.get_tiles_legend(tiles_map[i][j]).y / 32;
                matrizGrilla[i][j] = {fila, columna};
                

                matrizGrillaSpawns[i][j] = static_cast<int>(game_map[i][j]);
            }
        }
        

        crearGrillaVisual(filas, columnas);
        

        actualizarGrillaVisualDesdeMatriz();
        
        qDebug() << "Matriz cargada exitosamente desde YAML";
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
                            QString("Error al cargar el archivo YAML: %1").arg(e.what()));
        qDebug() << "Error al cargar YAML:" << e.what();
    }
}

void EditorWindow::actualizarGrillaVisualDesdeMatriz() {
    qDebug() << "Actualizando grilla visual desde matriz de datos";
    
    QPixmap originalPixmap("/var/taller/gfx/tiles/dust.bmp");
    if (originalPixmap.isNull()) {
        qDebug() << "Error: No se pudo cargar el tileset para actualizar la grilla";
        return;
    }
    
    int tileWidth = 32;
    int tileHeight = 32;
    
    int filas = matrizGrilla.size();
    int columnas = matrizGrilla[0].size();
    
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < columnas; ++j) {
            if (grillaCeldas[i][j]) {
                
                int tileRow = matrizGrilla[i][j].first;
                int tileCol = matrizGrilla[i][j].second;
                if (tileRow == 0 && tileCol == 0) {
                    continue; 
                }
                
                int x = tileCol * tileWidth;
                int y = tileRow * tileHeight;
                
                if (x >= 0 && y >= 0 && 
                    x + tileWidth <= originalPixmap.width() && 
                    y + tileHeight <= originalPixmap.height()) {
                    
                    QPixmap tilePixmap = originalPixmap.copy(x, y, tileWidth, tileHeight);
                    
                    if (!tilePixmap.isNull()) {
                        grillaCeldas[i][j]->setPixmap(tilePixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    } else {
                        qDebug() << "Error al extraer tile en posición:" << i << j << "- Tile coords:" << tileRow << tileCol;
                    }
                } else {
                    qDebug() << "Coordenadas de tile fuera de rango:" << x << y << "en posición:" << i << j;
                        grillaCeldas[i][j]->clear();
                }
            }
        }
    }
    qDebug() << matrizGrilla[0][0].first << matrizGrilla[0][0].second;
    
    qDebug() << "Grilla visual actualizada correctamente";
}