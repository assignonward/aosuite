#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    restoreConfig();
    on_update_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWinCommon::restoreConfig - places the window at
 *   the same location and size on the desktop as when it was
 *   last closed.
 */
void MainWindow::restoreConfig()
{ QSettings settings;
  restoreGeometry( settings.value( "geometry" ).toByteArray() );
  restoreState   ( settings.value( "state"    ).toByteArray() );
  if ( settings.value( "ricey" ).toString().size() > 0 ) ui->ricey->setHtml( settings.value( "ricey" ).toString() );
  if ( settings.value( "notes" ).toString().size() > 0 ) ui->notes->setHtml( settings.value( "notes" ).toString() );
}

/**
 * @brief MainWinCommon::saveConfig - saves the window location
 *   and size on the desktop to be restored on next launch.
 */
void MainWindow::saveConfig()
{ QSettings settings;
  settings.setValue( "geometry", saveGeometry()      );
  settings.setValue( "state"   , saveState()         );
  settings.setValue( "ricey"   , ui->ricey->toHtml() );
  settings.setValue( "notes"   , ui->notes->toHtml() );
}

void MainWindow::on_save_clicked()
{ saveConfig(); }

void MainWindow::on_update_clicked()
{ init();
  firstPass();
  translateNotes();
  t.append( "\n" );
  translateRicey();
  showResults();
}

/**
 * @brief MainWindow::init - reset everything ready for a new translation
 */
void MainWindow::init()
{ v.clear();
  t.clear();
  notesNumChar.clear();
  notesList = ui->notes->toPlainText().split("\n",QString::SkipEmptyParts);
  riceyList = ui->ricey->toPlainText().split("\n",QString::SkipEmptyParts);
  maxNameLength = 0;
  maxNumLength  = 0;
}

/**
 * @brief MainWindow::showResults - update the translations window
 */
void MainWindow::showResults()
{ ui->translations->clear();
  if ( v.size() > 0 )
    t.append( "\n"+v );
  ui->translations->append( t );
}

/**
 * @brief MainWindow::firstPass - pick up some easy stuff for use in the later passes
 */
void MainWindow::firstPass()
{ foreach ( QString line, riceyList )
    { QStringList words = line.split(" ",QString::SkipEmptyParts);
      if ( words.size() > 1 )
        { if ( words.at(0).size() > maxNameLength )
            maxNameLength = words.at(0).size();
          if ( words.at(1).size() > maxNumLength )
            maxNumLength = words.at(1).size();
        }
       else
        { v.append( QString( "WARN: short line '%1' in Ricey Code Definitions.\n" ).arg( line ) );
        }
    }
  foreach ( QString line, notesList )
    { QStringList words = line.split(" ",QString::SkipEmptyParts);
      if ( words.size() > 3 )
        { if ( words.at(1).startsWith( "0x" ) )
            { if ( words.at(1).size() == 3 )
                { if ( words.at(3).size() == 1 )
                    { notesNumChar.insert( words.at(1).at(2), words.at(3).at(0) );
                    } else { v.append( QString( "WARN: 4th word in Code Notes '%1' isn't length 1.\n" ).arg( line ) ); }
                } else { v.append( QString( "WARN: 2nd word in Code Notes '%1' isn't length 3.\n" ).arg( line ) ); }
            } else { v.append( QString( "WARN: 2nd word in Code Notes '%1' doesn't start with 0x.\n" ).arg( line ) ); }
        } else { v.append( QString( "WARN: line '%1' in Code Notes doesn't have 4 or more words.\n" ).arg( line ) ); }
    }
}

/**
 * @brief MainWindow::translateRicey - also check for correctness as we go
 */
void MainWindow::translateRicey()
{ foreach ( QString line, riceyList )
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
                  QString name = lList.at(0);
                  while ( name.size() < maxNameLength )
                    name.append( QChar(' ') );
                  QString num = lList.at(1);
                  while ( num.size() < maxNumLength )
                    num.prepend( QChar(' ') );
                  t.append( QString( "#define KTC_%1 %2 %3\n" ).arg( name ).arg( num ).arg( line.mid(ics) ) );
                }
            }
        }
    }
}


/**
 * @brief MainWindow::rulesCheck - operating on the ricey code definitions lines
 * @param name - code name
 * @param num  - code number
 * @return true if the rules are met
 */
bool MainWindow::rulesCheck( QString name, QString num )
{ if ( name.size() < 2 )
    { v.append( QString( "rulesCheck: name '%1' too short.\n" ).arg( name ) );
      return false;
    }
  if ( num.size() < 4 )
    { v.append( QString( "rulesCheck: num '%1' too short.\n" ).arg( num ) );
      return false;
    }
  if ( !num.startsWith( "0x" ) )
    { v.append( QString( "rulesCheck: num '%1' does not start with 0x.\n" ).arg( num ) );
      return false;
    }
  QChar numlc = num.at( num.size() - 1 );
  if ( !notesNumChar.keys().contains( numlc ) )
    { v.append( QString( "rulesCheck: num '%1' does not end with a recognized code.\n" ).arg( num ) );
      return false;
    }
  QChar namelc = name.at( name.size() - 1 );
  if ( notesNumChar[numlc] != namelc )
    { v.append( QString( "rulesCheck: name '%1' does not end with the expected character %2.\n" ).arg( name ).arg( notesNumChar[numlc] ) );
      return false;
    }
  QByteArray ba = QByteArray::fromHex( num.mid(2).toUtf8() );
  if ( ba.size() < 1 )
    { v.append( QString( "rulesCheck: '%1' does not translate as bytes of hex code.\n" ).arg( num ) );
      return false;
    }
  if ( ba.size() > 7 )
    { v.append( QString( "rulesCheck: '%1' too long for an AO Rice Code (%2 bytes, max is 7).\n" ).arg( num ).arg( ba.size() ) );
      return false;
    }
  if ( !validRice( ba ) )
    { v.append( QString( "rulesCheck: '%1' is not a valid Rice code, all bytes except the last must set the 0x80 bit.\n" ).arg( num ) );
      return false;
    }
  return true;
}

/**
 * @brief MainWindow::validRice
 * @param ba - byte array to check
 * @return true if ba is a valid rice code
 */
bool  MainWindow::validRice( const QByteArray &ba )
{ if ( ba.size() < 1 ) return false;
  if ( ba.size() > 7 ) return false;
  if ( (ba.at( ba.size() - 1 ) & 0x80) != 0 ) return false;
  if ( ba.size() == 1 ) return true;
  for ( int i = 0; i < ba.size() - 1; i++ )
    if ( (ba.at(i) & 0x80) == 0 ) return false;
  return true;
}


/**
 * @brief MainWindow::translateNotes - write the .h code defines for the notes (Ricey Data Types)
 */
void MainWindow::translateNotes()
{ foreach ( QString line, notesList )
    { t.append( QString( "#define RDT_%1\n" ).arg(line) );
    }
}
