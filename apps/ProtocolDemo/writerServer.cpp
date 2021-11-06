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
#include "writerServer.h"

WriterServer::WriterServer(QWidget *cw) :
    QGroupBox(cw),
    ui(new Ui::WriterServer)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      cw->layout()->setContentsMargins( 0,0,0,0 );
    }
  pa = new ProtocolActor( RCD_actorWriterServer_o, this );
  connect( pa, SIGNAL( newName(QString)           ), ui->wsProtocol, SLOT( setText(QString) ) );
  connect( pa, SIGNAL( transactionRecord(QString) ), ui->wsLog     , SLOT( append(QString)  ) );
  connect( pa, SIGNAL( newProtocolSet()           )                , SLOT( newProtocolSet() ) );
}

WriterServer::~WriterServer()
{ delete ui; }

/**
 * @brief WriterServer::newProtocolSet
 */
void WriterServer::newProtocolSet()
{ ui->wsIdGroup->setVisible( pa->sendableContents.contains( (RiceyInt)RCD_serverId_b ) );
}

/**
 * @brief WriterServer::receiveRequest - process it and generate a response
 * @param req - request from writer client
 */
void WriterServer::receiveRequest( QByteArray req )
{ pa->emit transactionRecord( QString("receiveRequest(%1)").arg( QString::fromUtf8( req.toHex() ) ) );
  RiceyInt reqTyp = riceToInt( req );
  BlockObjectMap bom = pa->extract( req );
  BaoSerial resp;
  switch ( reqTyp )
    { case RCD_writeRequest_o:
        // TODO: process bom, compose resp
        break;

      default:
        qWarning( "unrecognized request type %llx", reqTyp );
    }
  emit sendResponse( resp );
}
