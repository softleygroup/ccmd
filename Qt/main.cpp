#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QDebug>

//#include "../../Code/ccmd/CCMD/CCMD/Ion_trap.h"
//#include "../../Code/ccmd/CCMD/CCMD/ion_cloud.h"
//#include "../../Code/ccmd/CCMD/CCMD/integrator.h"

#include <string>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    //app.setQuitOnLastWindowClosed(true);

    //app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
