#ifndef FILEIO_H
#define FILEIO_H

#include <QString>

bool writeFile(QString filename, QString text) ;
bool appendFile(QString filename, QString text) ;
QString readFile(QString filename) ;

#endif // FILEIO_H

