#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QSizePolicy>
#include <QSpacerItem>
#include "editorWindow.h"  // Para ModoEditor

/* QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCrearMapaClicked();
    void onEditarMapaClicked();
    void onSalirClicked();

private:
    /* Ui::MainWindow *ui; */

    // Widgets personalizados para mejor control
    QWidget *centralWidget;
    QLabel *tituloLabel;
    QPushButton *crearMapaBtn;
    QPushButton *editarMapaBtn;
    QPushButton *salirBtn;

    void setupCustomUI();
    void actualizarFondo();
    void aplicarEstilosResponsivos();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINWINDOW_H
