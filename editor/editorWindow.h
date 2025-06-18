#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>

enum ModoEditor {
    CrearNuevoMapa,
    EditarMapaExistente
};

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
    ModoEditor modo;
    QWidget *centralWidget;
    QLineEdit *nombre_mapa;
    QLineEdit *cant_jugadores;
    QPushButton *editar_mapa_btn;
    QPushButton *volver_menu_btn;
    void configurarVistaSegunModo(ModoEditor modo);
    void setupCustomUIConfiguracionMapa();
    void setupCustomUIEleccionMapa();
    void actualizarFondo();
    void aplicarEstilosResponsivos();
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // EDITORWINDOW_H
