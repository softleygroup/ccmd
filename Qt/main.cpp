#include <QtGui/QApplication>

#include "mainwindow.h"

#include <QDebug>
#include <string>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
}
