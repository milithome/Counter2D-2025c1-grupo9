#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <yaml-cpp/yaml.h>
#include <QPair>
#include <QVector>
#include <QFile>
#include <QString>
#include <fstream>
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
#include <QProcess>
#include <QShortcut>



#include "ClickableLabel.h"
#include "common/utilities/map.h"


enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
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

    QWidget *menuInicialWidget;   // vista Ingreso de
    QWidget *seleccionSpawnPoints; // vista spawn
    QWidget *editorMapaWidget;    // vista editarMapaNuevo
    QWidget *editorMapaExistenteWidget; // vista editarMapaExistente

    ModoEditor modo;
    QWidget *centralWidget;
    QLineEdit *nombre_mapa;
    QLabel* tituloCantJugadores;
    QLineEdit *cant_jugadores;
    
    QPushButton *editar_mapa_btn;
    QPushButton *volver_menu_btn;

    QString nombreArchivoActual;  
    QString rutaArchivoActual;    


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

    QVector<QVector<QLabel*>> grillasCeldasSpawns; 

    QVector<QVector<int>> matrizGrillaSpawns;

    QList<ClickableLabel*> iconosActivosSpawns; 
    QPair<int, int> coordenadasSeleccionadasSpawns;


    ClickableLabel* iconoSeleccionadoSpawns = nullptr; 

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

    void inicializarEditorMapa(int filas, int columnas, bool esNuevoMapa);

    QMenuBar *miMenuBar;
    QMenu *fileMenu;
    QAction *guardarAction;
    QAction *guardarSalirAction;

    QComboBox* categoriaCombo;

    QString bloqueSeleccionado; 

    QSignalMapper* iconMapper = nullptr;
    QHBoxLayout* iconosLayout = nullptr;  
    
    QPixmap pixmapSeleccionado;             
    QList<ClickableLabel*> iconosActivos; 

    QVector<QVector<QLabel*>> grillaCeldas;

    QVector<QVector<QPair<int, int>>> matrizGrilla; 
    QPair<int, int> coordenadasSeleccionadas;

    QGridLayout* gridLayout = nullptr;

    ClickableLabel* iconoSeleccionado = nullptr;


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


    //-------------------------------------------------------------------------

    QString comboBoxStyle;

    QComboBox* mapasExistentes;

    QPixmap tiles = QPixmap("/var/taller/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> solid_blocks = {{2,4},{2,5},{2,6},{2,7},{9,2},{9,3},{9,4},};

    void cargarArchivosYamlEnComboBox(QComboBox* comboBox);
    void cargarMatrizDesdeYaml();
    MapData crearMapData();
    void diagnosticarDirectorio();
    void limpiarGrillaExistente();
    void crearGrillaVisual(int filas, int columnas);
    void actualizarGrillaVisualDesdeMatriz();


protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
};

#endif // EDITORWINDOW_H