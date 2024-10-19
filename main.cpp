#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "../Lib/supportfunctions.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setApplicationName("Parse Web") ;
    a.setApplicationVersion(GITHASH) ;

    MainWindow w;
    w.show();

    w.parseIni(argc, argv) ;
    return a.exec();
}
