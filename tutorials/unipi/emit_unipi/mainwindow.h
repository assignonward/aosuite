#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "unipiemitter.h"
#include "propertyobject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit  MainWindow(QWidget *parent = 0);
             ~MainWindow();
        void  closeEvent(QCloseEvent *event);
        void  readColors();

signals:

public slots:
        void  readyToEmit();
        void  on_setColor_clicked();
        void  on_sendAll_clicked() { po->send_PI(); }

public:
     Ui::MainWindow *ui;
       UniPiEmitter *uniPiEmitter;
     PropertyObject *po;
};

#endif // MAINWINDOW_H
