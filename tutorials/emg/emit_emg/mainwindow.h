/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "emgemitter.h"

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

signals:

public slots:
        void  readyToEmit();
        void  on_periodSpinBox_valueChanged( int p ) { timer.setInterval( p ); }
        void  on_runCheckBox_toggled( bool r ) { if ( r ) timer.start(); else timer.stop(); }

public:
     Ui::MainWindow *ui;
         EmgEmitter *emgEmitter;
             QTimer  timer;
};

#endif // MAINWINDOW_H
