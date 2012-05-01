#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QProgressBar>

class Ion_cloud;
class Ion_trap;
class RESPA_integrator;
class Trap_params;
class Cloud_params;
class Integration_params;
class Hist3D;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void setParticles(const Ion_cloud& ic);
    void startSim();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_ImageData_checkBox_toggled(bool checked);

    void on_UpdateImage_button_clicked();

    void on_vrf_spinBox_valueChanged(double arg1);

    void on_vend_spinBox_valueChanged(double arg1);

    void on_saveImage_Button_clicked();

    void on_action_Save_image_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *statLabel;
    QLabel *statLabel_sim;

    QProgressBar *statProgress;

    Ion_cloud *cloud;
    Ion_trap *trap;
    RESPA_integrator *integrator;
    Trap_params *trap_params;
    Cloud_params *cloud_params;
    Integration_params *integrator_params;
    Hist3D *hist;

    bool simRunning;
    bool collectImageData;

    QImage ccdImage;
    void update_ccdImage();
    QString image_fileName;
    void saveImage();
};

#endif // MAINWINDOW_H
