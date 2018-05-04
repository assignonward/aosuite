#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{ ui->setupUi(this);
  uniPiEmitter = new UniPiEmitter( this );
  po = new PropertyObject( this );
  readColors();
  connect(uniPiEmitter,SIGNAL(exchangeDeclared()),SLOT(readyToEmit()));
  connect(po,SIGNAL(messageToSend(QByteArray)),uniPiEmitter,SLOT(sendMessage(QByteArray)));

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

  uniPiEmitter->connectHost();
}

MainWindow::~MainWindow()
{ delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{ uniPiEmitter->m_client.disconnectFromHost();
  uniPiEmitter->deleteLater();
  QMainWindow::closeEvent(event);
}

void MainWindow::readyToEmit()
{ qDebug("MainWindow::readyToEmit()");
  ui->frame->setEnabled( true );
  po->send_PI(); // Sends the whole property tree
}

void MainWindow::on_setColor_clicked()
{ QString cs;
  int n = ui->activeN->value();
  switch ( n )
    { case 1: cs = ui->color1->text(); break;
      case 2: cs = ui->color2->text(); break;
      case 3: cs = ui->color3->text(); break;
      case 4: cs = ui->color4->text(); break;
      case 5: cs = ui->color5->text(); break;
      case 6: cs = ui->color6->text(); break;
      case 7: cs = ui->color7->text(); break;
      case 8: cs = ui->color8->text(); break;
    }
  QColor curColor( cs );
  QColor newColor = QColorDialog::getColor( curColor, this );
  if ( newColor.isValid() )
    { cs = newColor.name();
      switch ( n )
        { case 1: ui->color1->setText( cs ); po->set_PI_colors_0( (int)newColor.rgba() ); break;
          case 2: ui->color2->setText( cs ); po->set_PI_colors_1( (int)newColor.rgba() ); break;
          case 3: ui->color3->setText( cs ); po->set_PI_colors_2( (int)newColor.rgba() ); break;
          case 4: ui->color4->setText( cs ); po->set_PI_colors_3( (int)newColor.rgba() ); break;
          case 5: ui->color5->setText( cs ); po->set_PI_colors_4( (int)newColor.rgba() ); break;
          case 6: ui->color6->setText( cs ); po->set_PI_colors_5( (int)newColor.rgba() ); break;
          case 7: ui->color7->setText( cs ); po->set_PI_colors_6( (int)newColor.rgba() ); break;
          case 8: ui->color8->setText( cs ); po->set_PI_colors_7( (int)newColor.rgba() ); break;
        }
    }
}

void MainWindow::readColors()
{ QColor c;
  c.setNamedColor( ui->color1->text() ); po->set_PI_colors_0( (int32_t)c.rgba() );
  c.setNamedColor( ui->color2->text() ); po->set_PI_colors_1( (int32_t)c.rgba() );
  c.setNamedColor( ui->color3->text() ); po->set_PI_colors_2( (int32_t)c.rgba() );
  c.setNamedColor( ui->color4->text() ); po->set_PI_colors_3( (int32_t)c.rgba() );
  c.setNamedColor( ui->color5->text() ); po->set_PI_colors_4( (int32_t)c.rgba() );
  c.setNamedColor( ui->color6->text() ); po->set_PI_colors_5( (int32_t)c.rgba() );
  c.setNamedColor( ui->color7->text() ); po->set_PI_colors_6( (int32_t)c.rgba() );
  c.setNamedColor( ui->color8->text() ); po->set_PI_colors_7( (int32_t)c.rgba() );
}
