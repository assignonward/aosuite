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
  logReceiver = new EmgReceiver();
  connect( ui->echoCheckBox, SIGNAL(toggled(bool)), logReceiver, SLOT(setEcho(bool)) );
  connect( logReceiver, SIGNAL(newPacketTotal(int))    , ui->nPacketsLabel , SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(newByteTotal(int)  )    , ui->nBytesLabel   , SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(newPacketSize(int) )    , ui->lastBytesLabel, SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(packetContents(QString)), ui->plainTextEdit , SLOT(appendPlainText(QString)) );
  logReceiver->setEcho( ui->echoCheckBox->isChecked() );
  logReceiver->start( QStringList("raw") );
}

MainWindow::~MainWindow()
{
    delete ui;
}
