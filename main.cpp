#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Parse Web") ;

    MainWindow w;
    w.show();

    w.parseIni(argc, argv) ;
    return a.exec();
}
