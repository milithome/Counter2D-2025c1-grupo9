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

#include <iostream>


#include "ClickableLabel.h"
#include "common/utilities/map.h"

enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
};

enum TipoGrid { GRID_TERRENO, GRID_SPAWNS };

const int MIN_FILAS = 10;
const int MAX_FILAS = 55;
const int MIN_COLUMNAS = 10;
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

    void limpiarIconosAnteriores();
    void crearIconosPisos();
    void crearIconosMuros();
    void crearIconosSpawns();

private:

    QStackedWidget* stackedWidget;

    QWidget *menuInicialWidget;   // vista 1
    QWidget *editorMapaWidget;    // vista 2

    ModoEditor modo;
    QWidget *centralWidget;
    QLineEdit *nombre_mapa;
    QLineEdit *cant_jugadores;
    int jugadoresMaximos;
    QPushButton *editar_mapa_btn;
    QPushButton *volver_menu_btn;

    QMenuBar *miMenuBar;
    QMenu *fileMenu;
    QAction *guardarAction;
    QAction *borrarAction;
    QAction *borrarTodoAction;

    QComboBox* categoriaCombo;

    QString bloqueSeleccionado; // Recurso del bloque elegido

    QSignalMapper* iconMapper = nullptr;
    QHBoxLayout* iconosLayout = nullptr;  // Layout solo para íconos
    
    QList<ClickableLabel*> iconosActivos; // Para mantener referencia a los íconos activos

    QVector<QVector<QLabel*>> grillaCeldas; // Grilla del mapa


    QPixmap tiles = QPixmap(":/assets/gfx/tiles/dust.bmp");

    QSet<QPair<int, int>> solid_blocks;
    QVector<QVector<QPair<int, int>>> matrizGrilla; // Matriz para almacenar las coordenadas
    QVector<QVector<int>> matrizGrillaSpawns;
    QPair<int, int> coordenadasSeleccionadas;

    QGridLayout* gridLayout = nullptr;

    bool dragging;

    ClickableLabel* iconoSeleccionado = nullptr; // Ícono actualmente seleccionado
    QPushButton* agregarFilaBtn;
    QPushButton* agregarColumnaBtn;
    QPushButton* eliminarFilaBtn;
    QPushButton* eliminarColumnaBtn;

    QString nombreArchivoActual;  // Para guardar el nombre del archivo YAML actual
    QString rutaArchivoActual;    // Para guardar la ruta completa del archivo

    QPixmap pixmapSeleccionado;              // Pixmap del bloque seleccionado

    void configurarVistaSegunModo(ModoEditor modo);
    void setupCustomUIConfiguracionMapa();
    void setupCustomUIEleccionMapa();
    void actualizarFondo();
    void aplicarEstilosResponsivos();
    void inicializarEditorMapa();
    void guardarMapa();
    void borrarSeleccionados();
    void borrarTodo();
    void actualizarSeleccionVisual(ClickableLabel* nuevoSeleccionado);
    void actualizarTamanoGridWidget();

    void crearGrilla(int filas, int columnas,
                               QWidget *&gridWidget,
                               QGridLayout *&gridLayout,
                               QVector<QVector<QLabel*>> &grilla,
                               TipoGrid tipo);

    void agregarFila();
    void agregarColumna();
    void eliminarFila();
    void eliminarColumna();
    void actualizarEstadoBotonesDimensiones();
    std::pair<int, int> calcularDimensiones();

    void crearArchivoYamlInicial();
    void guardarProgresoEnYaml();
    MapData crearMapData();
   
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // EDITORWINDOW_H
