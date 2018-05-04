/*************************************************
*       Copyright © 2016, Medtronic, Inc.        *
* Confidential, Proprietary, All Rights Reserved *
*************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{ ui->setupUi(this);
  ui->emitButton->setEnabled( false );
  ui->runCheckBox->setChecked( false );
  ui->runCheckBox->setEnabled( false );
  on_periodSpinBox_valueChanged( ui->periodSpinBox->value() );
  emgEmitter = new EmgEmitter( this );
  connect(ui->bytesSpinBox ,SIGNAL(valueChanged(int)),emgEmitter,SLOT(setBytes(int)) );
  connect(ui->offsetSpinBox,SIGNAL(valueChanged(int)),emgEmitter,SLOT(setOffset(int)));
  connect(ui->echoCheckBox ,SIGNAL(toggled(bool))    ,emgEmitter,SLOT(setEcho(bool)) );
  emgEmitter->setBytes(  ui->bytesSpinBox ->value()   );
  emgEmitter->setOffset( ui->offsetSpinBox->value()   );
  emgEmitter->setEcho(   ui->echoCheckBox->isChecked());
  connect(emgEmitter,SIGNAL(exchangeDeclared()),SLOT(readyToEmit()));
  emgEmitter->connectHost();
  connect(ui->emitButton,SIGNAL(clicked(bool)),emgEmitter,SLOT(sendMessage()));
  connect(&timer,        SIGNAL(timeout()),    emgEmitter,SLOT(sendMessage()));
}

MainWindow::~MainWindow()
{ delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{ emgEmitter->m_client.disconnectFromHost();
  emgEmitter->deleteLater();
  QMainWindow::closeEvent(event);
}

void MainWindow::readyToEmit()
{ qDebug("MainWindow::readyToEmit()");
  ui->emitButton->setEnabled( true );
  ui->runCheckBox->setEnabled( true );
}
