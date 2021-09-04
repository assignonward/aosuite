#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_update_clicked()
{ ui->translations->clear();
  v.clear(); t.clear();
  QStringList rList = ui->ricey->toPlainText().split("\n",QString::SkipEmptyParts);
  foreach ( QString line, rList )
    { qint32 ics = line.indexOf( "//" );
      if ( ics < 0 )
        v.append( "WARN: missing // in: "+line+"\n" );
       else
        { QString left = line.mid( 0, ics );
          QStringList lList = left.split(" ",QString::SkipEmptyParts);
          if ( lList.size() != 2 )
            v.append( QString( "WARN: %1 words before // in: %2\n" ).arg( lList.size() ).arg( line ) );
          if ( lList.size() > 1 )
            { if ( !rulesCheck( lList.at(0), lList.at(1) ) )
                v.append( QString( "WARN: rules check failed in: %1\n" ).arg( line ) );
               else
                { // Temporary output for warm fuzzies
                  t.append( QString( "#define KTC_%1 %2 %3\n" ).arg( lList.at(0) ).arg( lList.at(1) ).arg( line.mid(ics) ) );
                }

            }
        }
    }

  t.append( ui->notes->toPlainText() );

  t.append( v );
  ui->translations->append( t );
}

/**
 * @brief MainWindow::rulesCheck - operating on the ricey code definitions lines
 * @param name - code name
 * @param num  - code number
 * @return true if the rules are met
 */
bool MainWindow::rulesCheck( QString name, QString num )
{ if ( name.size() < 2 )
    { v.append( QString( "rulesCheck: name '%1' too short" ).arg( name ) );
      return false;
    }
  if ( num.size() < 4 )
    { v.append( QString( "rulesCheck: num '%1' too short" ).arg( num ) );
      return false;
    }
  if ( !num.startsWith( "0x" ) )
    { v.append( QString( "rulesCheck: num '%1' does not start with 0x" ).arg( num ) );
      return false;
    }
  // More to come
  return true;
}
