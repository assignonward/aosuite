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
#include "mainwindow.h"
#include "aboutform.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    MainWinCommon(parent),
    ui(new Ui::MainWindow)
{ ce = new CryptoEngine(     this );
  ae = new AssetsEngine( ce, this );
  ui->setupUi(this);
  new AboutForm( ui->aboutTab );
  cf = new CryptoForm ( ui-> cryptoTab, this, ce     );
  af = new AssetForm  ( ui-> assetsTab, this, ce, ae );
  gf = new GenesisForm( ui->genesisTab, this,     ae );
  connect( this, SIGNAL(message(QString)),gf->ui->logMessages,SLOT(appendPlainText(QString)) );
  messageConnected = true;
  restoreConfig();
}

MainWindow::~MainWindow()
{ delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{ saveConfig();
  QApplication::processEvents( QEventLoop::AllEvents,100 );
  QMainWindow::closeEvent(event);
}

/**
 * @brief MainWindow::restoreConfig - in addition to the window size and
 *   placement saved in MainWinCommon, this app is also restoring assets.
 */
void MainWindow::restoreConfig()
{ MainWinCommon::restoreConfig();
  QSettings settings;
  // assets = settings.value( "assets" ).toByteArray();
}

/**
 * @brief MainWindow::saveConfig - in addition to the window size and
 *   placement saved in MainWinCommon, this app is also saving assets.
 */
void MainWindow::saveConfig()
{ MainWinCommon::saveConfig();
  QSettings settings;
  // settings.setValue( "assets", assets.toDataItem() );
}
