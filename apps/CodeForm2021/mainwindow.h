#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
        MainWindow(QWidget *parent = nullptr);
       ~MainWindow();
  bool  rulesCheck( QString, QString );

public slots:
  void  on_update_clicked();

public:
    QString t; // translation
    QString v; // violations

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
