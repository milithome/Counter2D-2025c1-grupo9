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


#include "ClickableLabel.h"

enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
};

// Enumeración para tipos de bloques
enum TipoBloque {
    VACIO = 0,
    PISO = 1,
    MURO = 2,
    SPAWN = 3,
    ZONA = 4
};

// Estructura para guardar información completa de cada celda
struct InfoCelda {
    TipoBloque tipo;
    int subTipo;  // Para diferenciar entre diferentes pisos/muros/etc
    QString texturePath; // Path de la textura si es necesario

    InfoCelda() : tipo(VACIO), subTipo(0), texturePath("") {}
    InfoCelda(TipoBloque t, int st = 0, QString path = "")
        : tipo(t), subTipo(st), texturePath(path) {}
};

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
    // En el archivo editorWindow.h, agregar estos miembros privados:
    QList<ClickableLabel*> iconosActivos; // Para mantener referencia a los íconos activos

    QVector<QVector<QLabel*>> grillaCeldas; // Grilla del mapa

    QGridLayout* gridLayout = nullptr;

    bool dragging;

    ClickableLabel* iconoSeleccionado = nullptr; // Ícono actualmente seleccionado
    QPushButton* agregarFilaBtn;
    QPushButton* agregarColumnaBtn;
    QPushButton* eliminarFilaBtn;
    QPushButton* eliminarColumnaBtn;


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

    void agregarFila();
    void agregarColumna();
    void eliminarFila();
    void eliminarColumna();
    void actualizarEstadoBotonesDimensiones();
    std::pair<int, int> calcularDimensiones();
    void pintarCelda(QLabel* celda);


    QVector<QVector<InfoCelda>> datosGrilla; // Matriz paralela con información de bloques
    TipoBloque tipoSeleccionado;             // Tipo actualmente seleccionado
    int subTipoSeleccionado;                 // Subtipo actualmente seleccionado
    QPixmap pixmapSeleccionado;              // Pixmap del bloque seleccionado
    QAction* cargarAction;                   // Acción para cargar mapas

    // Funciones para manejo de datos
    //void inicializarDatosGrilla(int filas, int columnas);
    //void actualizarCelda(int fila, int columna, TipoBloque tipo, int subTipo);
    //InfoCelda obtenerInfoCelda(int fila, int columna);
    //void redimensionarDatosGrilla();
    //void guardarMapaEnArchivo();
    //void cargarMapaDesdeArchivo(const QString& nombreArchivo);
    //void mostrarEstadoGrillaEnConsola(); // Para debug
    //QString tipoAString(TipoBloque tipo);
    //QPixmap obtenerPixmapParaTipo(TipoBloque tipo, int subTipo);


protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // EDITORWINDOW_H
