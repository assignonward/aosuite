/* MIT License
 *
 * Copyright (c) 2021 Assign Onward
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
#include "protocolDemo.h"
#include <QDir>

ProtocolDemo::ProtocolDemo( QWidget *cw ) :
    QScrollArea(cw),
    ui(new Ui::ProtocolDemo)
{ ui->setupUi(this);
  initReadFile();
  if ( cw )
    { QVBoxLayout *vb = new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      vb->setContentsMargins( 0,0,0,0 );
    }
  connect( this, SIGNAL( setProtocol(BaoSerial) ), &wc, SLOT( setProtocol(BaoSerial) ) );
  connect( this, SIGNAL( setProtocol(BaoSerial) ), &rc, SLOT( setProtocol(BaoSerial) ) );
  connect( this, SIGNAL( setProtocol(BaoSerial) ), &ws, SLOT( setProtocol(BaoSerial) ) );
  connect( this, SIGNAL( setProtocol(BaoSerial) ), &rs, SLOT( setProtocol(BaoSerial) ) );
  connect( &wc , SIGNAL( newName(QString) ), ui->wcProtocol, SLOT( setText(QString)  ) );
  connect( &rc , SIGNAL( newName(QString) ), ui->rcProtocol, SLOT( setText(QString)  ) );
  connect( &ws , SIGNAL( newName(QString) ), ui->wsProtocol, SLOT( setText(QString)  ) );
  connect( &rs , SIGNAL( newName(QString) ), ui->rsProtocol, SLOT( setText(QString)  ) );
}

ProtocolDemo::~ProtocolDemo()
{ delete ui; }

/**
 * @brief ProtocolDemo::initReadFile - readFile is a comboBox which specifies
 *   a resource file to select a protocol from when Set is clicked.
 */
void  ProtocolDemo::initReadFile()
{ ui->readFile->clear();
  QDir d(":/protocols");
  QFileInfoList fil = d.entryInfoList();
  foreach( QFileInfo fi, fil )
    { QString fn = fi.fileName();
      if ( fn.endsWith( ".bao" ) )
        ui->readFile->addItem( fn.mid( 0, fn.size()-4) );
    }
}

void  ProtocolDemo::on_set_clicked()
{ QString readFile;
  readFile = ":/files/"+ui->readFile->currentText()+".bao";
  if ( readFile.size() < 1 )
    return;
  QFile file(readFile);
  if ( !file.open(QIODevice::ReadOnly) )
    { qWarning( "could not open %s for reading", readFile.toUtf8().data() );
      return;
    }
   QFileInfo fi = QFileInfo( file );
  Utf8String fn = fi.fileName().toUtf8();
   BaoSerial bs = file.readAll();
  if ( bs.size() > 0 )
    emit setProtocol( bs );
   else
    qWarning( "read no data from %s", fn.data() );
}

void WriterClient::receiveResponse( QByteArray resp )
{ (void)resp;
}

void ReaderClient::receiveResponse( QByteArray resp )
{ (void)resp;
}

void WriterServer::receiveRequest( QByteArray req )
{ (void)req;
}

void ReaderServer::receiveRequest( QByteArray req )
{ (void)req;
}

