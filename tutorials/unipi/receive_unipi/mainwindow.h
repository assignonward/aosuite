#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "unipireceiver.h"
#include "propertyobject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateColors();

private:
    Ui::MainWindow *ui;
       UniPiReceiver *logReceiver;
      PropertyObject *po;
};

#endif // MAINWINDOW_H
