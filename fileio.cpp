
#include "fileio.h"
#include <QFile>
#include <QTextStream>
#include <QString>

bool writeFile(QString filename, QString text)
{
     QFile file(filename);
     if (!file.open(QIODevice::WriteOnly)) {
         return false;
     }
     QTextStream out(&file);
     out << text ;
    return true ;
}

QString readFile(QString filename)
{
    QString contents ;
    QFile file(filename);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return QString("ERROR READING FILE");
    contents = QString::fromLatin1(file.readAll().data());
    return contents;
}
