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
#include "readerClient.h"

ReaderClient::ReaderClient(QWidget *cw) :
    QGroupBox(cw),
    ui(new Ui::ReaderClient)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      cw->layout()->setContentsMargins( 0,0,0,0 );
    }
  pa = new ProtocolActor( RCD_actorReaderClient_o, this );
  connect( pa           , SIGNAL( newName(QString)           ), ui->rcProtocol, SLOT( setText(QString)  ) );
  connect( pa           , SIGNAL( transactionRecord(QString) ), ui->rcLog     , SLOT( append(QString)   ) );
  connect( pa           , SIGNAL( newProtocolSet()           )                , SLOT( newProtocolSet()  ) );
  connect( ui->rcRequest, SIGNAL( clicked()                  )                , SLOT( sendReadRequest() ) );
}

ReaderClient::~ReaderClient()
{ delete ui; }

/**
 * @brief ReaderClient::newProtocolSet
 */
void ReaderClient::newProtocolSet()
{ ui->rcRequest          ->setVisible( pa->   sendableObTypes.contains( (RiceyInt)RCD_readRequest_o ) );
  ui->rcDataHandleGroup  ->setVisible( pa->  sendableContents.contains( (RiceyInt)RCD_recordId_i    ) );
  ui->rcIdGroup          ->setVisible( pa->  sendableContents.contains( (RiceyInt)RCD_userId_b      ) );
  ui->rcDataGroup        ->setVisible( pa->receivableContents.contains( (RiceyInt)RCD_recordText_s  ) );
  ui->rcBlockchainIdGroup->setVisible( false ); // TODO: define a protocol that includes blockchain id
  // qWarning( "ReaderClient::newProtocolSet()" );
}

/**
 * @brief ReaderClient::sendReadRequest - catches signal from the ui button
 */
void ReaderClient::sendReadRequest()
{ pa->emit transactionRecord("sendReadRequest()");
  if ( pa->pp == nullptr )
    { pa->emit transactionRecord("protocol not defined.");
      return;
    }
  // Compose bao based on protocol definition and ui contents and send it.
  // Unneeded components will be ignored by pa->compose
  BlockObjectMap inputs;
  BlockValueByteArray userId( ui->rcId->text().toUtf8() );             inputs.insert( RCD_userId_b  , &userId   );
  BlockValueInt64   recordId( ui->rcDataHandle->text().toLongLong() ); inputs.insert( RCD_recordId_i, &recordId );
  // more to come
  BaoSerial bao = pa->compose( RCD_readRequest_o, inputs );
  if ( bao.size() > 0 )
    emit sendRequest( bao );
   else
    qWarning( "problem when composing readRequest" );
}

/**
 * @brief ReaderClient::receiveResponse
 * @param resp - from server
 */
void ReaderClient::receiveResponse( QByteArray resp )
{ pa->emit transactionRecord("receiveResponse()");
  (void)resp;
  // TODO: show response/results on ui
}
