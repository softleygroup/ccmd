#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

#include "integrator.h"
//#include "cuda_integrator.h"

#include "hist3D.h"
#include "ccmd_image.h"

#include "trap_param_model.h"
#include "trap_param_delegate.h"

#include <QTreeView>
#include <QDebug>
#include <QImage>
#include <QFileDialog>

void setup_trap_treeView(QTreeView* treeView, Trap_param_model* trapModel);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()) );

    ui->action_Save_image->setEnabled(false);

    std::string path_to_config = "../config/";
    //std::string path_to_config = "/Users/mtb/Develop/ccmd/config/";
    std::string trap_param_file = path_to_config + "trap_config.txt";
    std::string ion_types_file = path_to_config + "ion_types.txt";
    std::string cloud_param_file = path_to_config + "ion_numbers.txt";
    std::string integrator_param_file = path_to_config +"integrator.txt";

    // Get simulation parameters from files
    trap_params = new Trap_params( trap_param_file );
    cloud_params = new Cloud_params( cloud_param_file, ion_types_file);
    integrator_params = new Integration_params( integrator_param_file );

    // Construct trap
    trap = new Cosine_trap (*trap_params);

    // Construct ion cloud
    cloud = new Ion_cloud(*trap, *cloud_params);

    // Construct integrator
    integrator = new RESPA_integrator(*trap, *cloud, *integrator_params);
    //integrator = new CUDA_integrator(*trap, *cloud, *integrator_params);

    // Construct histogram
    hist = new Hist3D;
    hist->set_bin_size(0.1);

    microscope_params = new Microscope_params();

    /*
    //  change all Ca to Xe
    while ( cloud->change_ion("calcium", "xenon") )
    {
        qDebug() << "Ion changed!\n";
    }
    */

    // Update GL display
    setParticles(*cloud);
    simRunning = false;

    // Set up image
    ccdImage = QImage(640,640,QImage::Format_RGB32);
    ccdImage.fill(Qt::black);
    ui->crystal_image_label->setPixmap( QPixmap::fromImage(ccdImage) );

    collectImageData = false;
    update_ccdImage();

    // Status bar
    statLabel = new QLabel(this);
    statLabel_sim = new QLabel(this);
    statProgress = new QProgressBar(this);

    statLabel_sim->setAlignment(Qt::AlignRight);
    ui->statusBar->addPermanentWidget(statLabel_sim);

    ui->statusBar->addPermanentWidget(statLabel);
    ui->statusBar->addPermanentWidget(statProgress);

    statProgress->hide();

    statusBar()->showMessage("Initialised",1000);

    // Tree view and model for trap parameters
    trapModel = new Trap_param_model( *trap_params );
    setup_trap_treeView(ui->trap_treeView, trapModel);
    connect(trapModel,SIGNAL(parameters_changed()),
            this,SLOT(sim_parameters_changed()));
}

MainWindow::~MainWindow()
{
    simRunning = false;
    startSim();
    qApp->processEvents();

    delete ui;
    delete trap;
    delete cloud;
    delete integrator;
    delete hist;
    delete trap_params;
    delete cloud_params;
    delete integrator_params;

}

void MainWindow::setParticles(const Ion_cloud& ic)
{
    ui->CrystalView->setParticles(ic);
}

void MainWindow::startSim()
{
    static int time;
    QString sim_Label("Time steps run: ");
    QString time_steps;
    time_steps.setNum(time);
    statLabel->setText( sim_Label + time_steps );

    //double dt = 1.213/10;

    while (simRunning)
    {
        integrator->evolve();
        setParticles(*cloud);

        if (collectImageData) {
            cloud->update_position_histogram(*hist);
        }

        time_steps.setNum(time);
        statLabel->setText( sim_Label + time_steps );

        qApp->processEvents();
        ++time;
    }

}

void MainWindow::on_startButton_clicked()
{
    simRunning = true;
    startSim();
}

void MainWindow::on_stopButton_clicked()
{
    simRunning = false;
}

void MainWindow::update_ccdImage()
{
    if (hist->get_number_bins() > 0) {

        statLabel->setText("Generating image");
        statProgress->show();

        // Compute new image from histrogram data
        Microscope_image mImage(ccdImage.width(),ccdImage.height(), *hist);
        //mImage.set_params(*microscope_params);
        //qApp->processEvents();

        while ( !mImage.is_finished() ) {
                   mImage.draw();
                   statProgress->setValue( mImage.get_progress() );
                   qApp->processEvents();
        }

        mImage.normalise();
        for (int i=0; i<ccdImage.width(); ++i) {
            for (int j=0; j<ccdImage.height(); ++j) {

                int x = i+1;
                int y = j+1;

                int intensity = 255*mImage.get_pixel(y, x);
                QRgb value = qRgb(intensity, intensity, intensity);
                ccdImage.setPixel(i,j,value);
            }
        }

        // Update screen image
        ui->crystal_image_label->setPixmap( QPixmap::fromImage(ccdImage) );

        // UI housekeeping
        ui->saveImage_Button->setEnabled(true);
        ui->action_Save_image->setEnabled(true);
        statLabel->setText(" ");
        statProgress->hide();
    }
}


void MainWindow::on_ImageData_checkBox_toggled(bool checked)
{
    collectImageData = !collectImageData;
}

void MainWindow::on_UpdateImage_button_clicked()
{
    update_ccdImage();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    simRunning = false;
}

void MainWindow::saveImage()
{
    // Save to jpg
    // Default directory is currently "/Users/mtb/Documents"
    if ( image_fileName.isEmpty() ) {
       image_fileName = QFileDialog::getSaveFileName(this,
                                                     tr("Save image"),
                                                     "/Users/mtb/Documents",
                                                     tr("Image files (*.jpg);;All Files (*)") );
    }

    if ( image_fileName.isEmpty() ) {
        return;
    } else {
        ccdImage.save(image_fileName,"jpg",100);
        statusBar()->showMessage("Image saved to: " + image_fileName,3000);
        ui->saveImage_Button->setEnabled(false);
        ui->action_Save_image->setEnabled(false);
    }
}

void MainWindow::on_saveImage_Button_clicked()
{
    saveImage();
}

void MainWindow::on_action_Save_image_triggered()
{
    saveImage();
}

void setup_trap_treeView(QTreeView* treeView, Trap_param_model* trapModel)
{
    // register model
    treeView->setModel(trapModel);

    // use custom delegate to edit parameters
    treeView->setItemDelegate(new Trap_param_delegate(treeView) );

    // expand, resize to each column, and then collapse
    treeView->expandAll();
    for (int column = 0; column < trapModel->columnCount(); ++column)
        treeView->resizeColumnToContents(column);
    treeView->collapseAll();
    treeView->expandToDepth(0);

    treeView->header()->setMovable(false);
    treeView->setAnimated(true);
    //treeView->setAlternatingRowColors(true);
}

void MainWindow::sim_parameters_changed()
{
    trap->update_trap_params();
    cloud->update_params();
}

void MainWindow::on_ResetHistogram_button_clicked()
{
    hist->reset();
}

void MainWindow::on_w0_doubleSpinBox_valueChanged(double arg1)
{
    microscope_params->w0 = ui->w0_doubleSpinBox->value();
}

void MainWindow::on_z0_doubleSpinBox_valueChanged(double arg1)
{
    microscope_params->z0 = ui->z0_doubleSpinBox->value();
}
