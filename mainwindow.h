#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "initypes.h"
#include "../Lib/iniconfig.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    IniConfig ini ;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool parseIni(int argc, char *argv[]) ;
    void configureApplication() ;

private slots:
    void on_dicSearchText_editingFinished();
    void on_dicResource_textChanged(const QString &arg1);
    void on_actionChange_Resource_triggered();
    void on_dicResource_returnPressed();
    void on_actionE_xit_triggered();

    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;
    QString toPercentEncoding(QString& src) ;
    QString ParseWebResponse(QString resp) ;
    QString Get(QString link) ;

};

#endif // MAINWINDOW_H
