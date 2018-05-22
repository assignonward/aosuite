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
#include "appname.h"
#include "mainwindow.h"
#include "random.h"
#include "singleapplication.h"

using namespace std;

MainWindow *mw;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{ switch (type)
    { default:
      case QtDebugMsg:
        // TODO: log a debug message QString(context.file), context.line, msg
        break;
      case QtInfoMsg:
        // TODO: log an info message QString(context.file), context.line, msg
        break;
      case QtWarningMsg:
        // TODO: log a warning message QString(context.file), context.line, msg
        break;
      case QtCriticalMsg:
        // TODO: log a critical message QString(context.file), context.line, msg
        break;
      case QtFatalMsg:
        // TODO: log a fatal message QString(context.file), context.line, msg
        abort();
    }
  (void)context; (void)msg;
}

int main(int argc, char *argv[])
{
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
    // qInstallMessageHandler(myMessageOutput);
    QObject::connect( &app, &SingleApplication::instanceStarted, mw, &MainWindow::additionalInstanceStarted );
    mw->show();
    int r = app.exec();
    // qInstallMessageHandler( 0 );
    return r;
}
