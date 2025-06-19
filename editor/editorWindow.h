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


#include "ClickableLabel.h"

enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
};
const int MIN_FILAS = 10;
const int MAX_FILAS = 25;
const int MIN_COLUMNAS = 10;
const int MAX_COLUMNAS = 25;


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

    QVector<QVector<QLabel*>> grillaCeldas; // Grilla del mapa
    QGridLayout* gridLayout = nullptr;
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
    void agregarFila();
    void agregarColumna();
    void eliminarFila();
    void eliminarColumna();
    void actualizarEstadoBotonesDimensiones();
    std::pair<int, int> calcularDimensiones();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // EDITORWINDOW_H
