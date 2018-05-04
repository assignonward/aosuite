/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "emgreceiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
       EmgReceiver *logReceiver;
};

#endif // MAINWINDOW_H
