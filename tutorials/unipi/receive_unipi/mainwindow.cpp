#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{ ui->setupUi(this);
  logReceiver = new UniPiReceiver();
  po = new PropertyObject( this );

  connect( ui->echoCheckBox, SIGNAL(toggled(bool)), logReceiver, SLOT(setEcho(bool)) );
  connect( logReceiver, SIGNAL(newPacketTotal(int))    , ui->nPacketsLabel , SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(newByteTotal(int)  )    , ui->nBytesLabel   , SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(newPacketSize(int) )    , ui->lastBytesLabel, SLOT(setNum(int)) );
  connect( logReceiver, SIGNAL(packetContents(QString)), ui->plainTextEdit , SLOT(appendPlainText(QString)) );
  logReceiver->setEcho( ui->echoCheckBox->isChecked() );

  connect( logReceiver, SIGNAL(newMessageReceived(QByteArray)), po, SLOT(newMessageReceived(QByteArray)) );

  // Visible argument for autoconnection follows:
  connect(po,SIGNAL(valueChanged_PI_levels_0(int)),ui->control1,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_1(int)),ui->control2,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_2(int)),ui->control3,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_3(int)),ui->control4,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_4(int)),ui->control5,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_5(int)),ui->control6,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_6(int)),ui->control7,SLOT(setValue(int)));
  connect(po,SIGNAL(valueChanged_PI_levels_7(int)),ui->control8,SLOT(setValue(int)));
  connect(ui->control1,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_0(int)));
  connect(ui->control2,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_1(int)));
  connect(ui->control3,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_2(int)));
  connect(ui->control4,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_3(int)));
  connect(ui->control5,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_4(int)));
  connect(ui->control6,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_5(int)));
  connect(ui->control7,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_6(int)));
  connect(ui->control8,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_levels_7(int)));

  connect(po,SIGNAL(valueChanged_PI_active(int)),ui->activeN,SLOT(setValue(int)));
  connect(ui->activeN,SIGNAL(valueChanged(int)),po,SLOT(slotSet_PI_active(int)));

  connect(po,SIGNAL(valueChanged_PI_decay(double)),ui->decay,SLOT(setValue(double)));
  connect(ui->decay,SIGNAL(valueChanged(double)),po,SLOT(slotSet_PI_decay(double)));

  connect(po,SIGNAL(valueChanged_PI_mark(bool)),ui->mark,SLOT(setChecked(bool)));
  connect(ui->mark,SIGNAL(toggled(bool)),po,SLOT(slotSet_PI_mark(bool)));

  connect(po,SIGNAL(valueChanged_PI_colors_0(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_1(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_2(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_3(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_4(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_5(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_6(int)),SLOT(updateColors()));
  connect(po,SIGNAL(valueChanged_PI_colors_7(int)),SLOT(updateColors()));

  logReceiver->start( QStringList("prop") );
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::updateColors - update all the color entries
 */
void MainWindow::updateColors()
{ QColor c;
  c = QColor( (QRgb)po->get_PI_colors_0() ); ui->color1->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_1() ); ui->color2->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_2() ); ui->color3->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_3() ); ui->color4->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_4() ); ui->color5->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_5() ); ui->color6->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_6() ); ui->color7->setText( c.name() );
  c = QColor( (QRgb)po->get_PI_colors_7() ); ui->color8->setText( c.name() );
}
