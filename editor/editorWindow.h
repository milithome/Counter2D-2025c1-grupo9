#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

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

private:
    ModoEditor modo;
    void configurarVistaSegunModo();
};

#endif // EDITORWINDOW_H
