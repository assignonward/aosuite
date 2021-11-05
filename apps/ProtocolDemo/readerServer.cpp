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
#include "readerServer.h"

ReaderServer::ReaderServer(QWidget *cw) :
    QGroupBox(cw),
    ui(new Ui::ReaderServer)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      cw->layout()->setContentsMargins( 0,0,0,0 );
    }
  pa = new ProtocolActor( RCD_actorReaderServer_o, this );
  connect( pa, SIGNAL( newName(QString)           ), ui->rsProtocol, SLOT( setText(QString) ) );
  connect( pa, SIGNAL( transactionRecord(QString) ), ui->rsLog     , SLOT( append(QString)  ) );
  connect( pa, SIGNAL( newProtocolSet()           )                , SLOT( newProtocolSet() ) );

}

ReaderServer::~ReaderServer()
{ delete ui; }

/**
 * @brief ReaderServer::newProtocolSet
 */
void ReaderServer::newProtocolSet()
{

}

/**
 * @brief ReaderServer::receiveRequest
 * @param req - request from reader client
 */
void ReaderServer::receiveRequest( QByteArray req )
{ pa->emit transactionRecord( QString("receiveRequest(%1)").arg( QString::fromUtf8( req.toHex() ) ) );
  (void)req;
  BaoSerial resp;
  // TODO: act on request and generate a response
  emit sendResponse( resp );
}
