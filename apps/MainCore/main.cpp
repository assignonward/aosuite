/* MIT License
 *
 * Copyright (c) 2018 Assign Onward
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <QApplication>
#include <QDateTime>
#include <QSettings>
#include <QFile>
#include "appname.h"
#include "mainwindow.h"
#include "random.h"
#include "singleapplication.h"
#include "stdio.h"

using namespace std;

MainWindow *mw;
QTextStream *logStream = NULL;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{ QString m;
  switch (type)
    { default:            m = "Def"; break;
      case QtDebugMsg:    m = "Dbg"; break;
      case QtInfoMsg:     m = "Inf"; break;
      case QtWarningMsg:  m = "Wrn"; break;
      case QtCriticalMsg: m = "Crt"; break;
      case QtFatalMsg:    m = "Ftl"; break;
    }
  if ( context.line > 0 )
    m.append( QString(  " %1:%2 %3" ).arg(context.file).arg( context.line ).arg( msg ) );
   else
    m.append( QString(  " %1" ).arg( msg ) );

  if ( mw )
    { if ( mw->messageConnected )
        emit mw->message( m );
    }
   else
    { printf( "%s\n", qPrintable(m) );
      fflush(stdout);
    }
  *logStream << m << endl;
  QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
  if ( type == QtFatalMsg )
    abort();
}

int main(int argc, char *argv[])
{   QFile logFile( "/home/mango/ao.log" );
    logFile.open( QIODevice::WriteOnly );
    logStream = new QTextStream( &logFile );
    qInstallMessageHandler(myMessageOutput);

    SingleApplication app(argc, argv);
    // Set names for QSettings use
    QCoreApplication::setOrganizationName("AssignOnward");
    QCoreApplication::setOrganizationDomain("assignonward.com");
    QCoreApplication::setApplicationName( APPNAME_SHORT );
    QSettings::setDefaultFormat(QSettings::IniFormat);

    rng.seed( QDateTime::currentMSecsSinceEpoch() );
    rng.rnd_uint64();
    rng.rnd_uint64();
    rng.rnd_uint64();

    mw = new MainWindow();
    QObject::connect( &app, &SingleApplication::instanceStarted, mw, &MainWindow::additionalInstanceStarted );
    mw->show();
    int r = app.exec();
    qInstallMessageHandler( nullptr );
    return r;
}
