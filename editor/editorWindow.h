#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H


#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QStackedWidget>
#include <QComboBox>
#include <QSignalMapper>
#include <QMouseEvent>
#include <QVector>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QScrollArea>
#include <QSpinBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>



#include "ClickableLabel.h"

enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
};

enum TipoGrid { GRID_TERRENO, GRID_SPAWNS };

struct dataMap{
    QString nombreMapa;
    int filas;
    int columnas;
    int playersMax;
    int cantZonaPlantable;
    //QVector<QVector<QPair<int, int>>> matrizG; // Matriz de bloques
    //QVector<QVector<int>> matrizSpawns; // Matriz de coordenadas de spawns
};

const int MIN_FILAS = 20;
const int MAX_FILAS = 55;
const int MIN_COLUMNAS = 20;
const int MAX_COLUMNAS = 55;


class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(ModoEditor modo, QWidget *parent = nullptr);
    ~EditorWindow();

private slots:
    void onCrearMapaClicked();
    void onEditarMapaClicked();
    void onSalirClicked();

    void onGuardarSpawnMapa();

    void limpiarIconosAnteriores(QHBoxLayout*& iconosLay, QList<ClickableLabel*>& iconosActivs, QSignalMapper*& iconMapperSs, ClickableLabel*& iconoSelec);
    void crearIconosPisos();
    void crearIconosMuros();
    void crearIconosSpawns();

private:

    QStackedWidget* stackedWidget;

    QWidget *menuInicialWidget;   // vista 1
    QWidget *seleccionSpawnPoints; // vista 2
    QWidget *editorMapaWidget;    // vista 3

    ModoEditor modo;
    QWidget *centralWidget;
    QLineEdit *nombre_mapa;
    QLineEdit *cant_jugadores;
    
    QPushButton *editar_mapa_btn;
    QPushButton *volver_menu_btn;

    dataMap datosMapa; // Estructura para almacenar los datos del mapa
    QString nombreArchivoActual;  // Para guardar el nombre del archivo YAML actual
    QString rutaArchivoActual;    // Para guardar la ruta completa del archivo


    void configurarVistaSegunModo(ModoEditor modo);
    void setupCustomUIConfiguracionMapa();
    void setupCustomUIEleccionMapa();
    void actualizarFondo();
    void aplicarEstilosResponsivos();
    void configurarCeldasParaDrops(QVector<QVector<QLabel*>>& celdas);
    bool manejoDragAndDrop(QObject* obj, QEvent* event);

    void inicializarSpawnsMapa();

    int jugadoresMaximos;
    int terroristas;
    int antiterroristas;
    int cantZonaPlantable;

    QComboBox* opciones;
    QGridLayout* gridLayoutSpawns = nullptr;

    QHBoxLayout* iconosLayoutSpawns = nullptr;
    QSignalMapper* iconMapperSpawns = nullptr;

    QVector<QVector<QLabel*>> grillasCeldasSpawns; // Grilla de celdas para spawns

    QVector<QVector<int>> matrizGrillaSpawns; // Matriz para almacenar las coordenadas de spawns

    QList<ClickableLabel*> iconosActivosSpawns; // Para mantener referencia a los íconos activos

    QPair<int, int> coordenadasSeleccionadasSpawns;


    ClickableLabel* iconoSeleccionadoSpawns = nullptr; // Ícono actualmente seleccionado

    QPushButton* guardarSpawnBtn;

    QString bloqueSeleccionadoSpawns;
    
    QPixmap pixmapSeleccionadoSpawns;  
    QPushButton* agregarFilaBtn;
    QPushButton* agregarColumnaBtn;
    QPushButton* eliminarFilaBtn;
    QPushButton* eliminarColumnaBtn;

    void verificarCondicionFinal();
    bool eventSpawns(QObject* obj, QEvent* event);
    bool alClickearIzquierdoSpawns(QLabel*& celda);
    bool alClickearDerechoSpawns(QLabel*& celda);
    void actualizarOpcionesDisponibles();

    void crearIconosZonaBomba();

    void inicializarEditorMapa(int filas, int columnas);

    QMenuBar *miMenuBar;
    QMenu *fileMenu;
    QAction *guardarAction;
    QAction *guardarSalirAction;
    QAction *borrarAction;
    QAction *borrarTodoAction;

    QComboBox* categoriaCombo;

    QString bloqueSeleccionado; // Recurso del bloque elegido

    QSignalMapper* iconMapper = nullptr;
    QHBoxLayout* iconosLayout = nullptr;  // Layout solo para íconos
    
    QPixmap pixmapSeleccionado;              // Pixmap del bloque seleccionado
    QList<ClickableLabel*> iconosActivos; // Para mantener referencia a los íconos activos

    QVector<QVector<QLabel*>> grillaCeldas; // Grilla del mapa

    QVector<QVector<QPair<int, int>>> matrizGrilla; // Matriz para almacenar las coordenadas
    QPair<int, int> coordenadasSeleccionadas;

    QGridLayout* gridLayout = nullptr;

    ClickableLabel* iconoSeleccionado = nullptr; // Ícono actualmente seleccionado


    bool eventEditar(QObject* obj, QEvent* event);
    bool alClickearIzquierdo(QLabel*& celda);
    bool alClickearDerecho(QLabel*& celda);
    void guardarMapa();
    void borrarSeleccionados();
    void borrarTodo();
    void actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado, 
                                             QList<ClickableLabel*>& iconsActivs, ClickableLabel*& iconSelec);
    void actualizarTamanoGridWidget();


    void agregarFila();
    void agregarColumna();
    void eliminarFila();
    void eliminarColumna();
    void actualizarEstadoBotonesDimensiones();

    void crearArchivoYamlInicial();
    void guardarProgresoEnYaml();
   
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
};

#endif // EDITORWINDOW_H