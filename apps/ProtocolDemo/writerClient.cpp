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
#include "writerClient.h"

WriterClient::WriterClient(QWidget *cw) :
    QGroupBox(cw),
    ui(new Ui::WriterClient)
{ ui->setupUi(this);
  if ( cw )
    { new QVBoxLayout( cw );
      cw->layout()->addWidget( this );
      cw->layout()->setContentsMargins( 0,0,0,0 );
    }
  pa = new ProtocolActor( RCD_actorWriterClient_o, this );
  connect( pa        , SIGNAL( newName(QString)           ), ui->wcProtocol, SLOT( setText(QString)   ) );
  connect( pa        , SIGNAL( transactionRecord(QString) ), ui->wcLog     , SLOT( append(QString)    ) );
  connect( pa        , SIGNAL( newProtocolSet()           )                , SLOT( newProtocolSet()   ) );
  connect( ui->wcSend, SIGNAL( clicked()                  )                , SLOT( sendWriteRequest() ) );
}

WriterClient::~WriterClient()
{ delete ui; }

/**
 * @brief WriterClient::newProtocolSet
 */
void WriterClient::newProtocolSet()
{ ui->wcSend             ->setVisible( pa->sendableObTypes .contains( (RiceyInt)RCD_writeRequest_o ) );
  ui->wcIdGroup          ->setVisible( pa->sendableContents.contains( (RiceyInt)RCD_userId_b       ) );
  ui->wcDataGroup        ->setVisible( pa->sendableContents.contains( (RiceyInt)RCD_recordText_s   ) );
  ui->wcBlockchainIdGroup->setVisible( false ); // TODO: define a protocol that includes blockchain id
  // qWarning( "WriterClient::newProtocolSet()" );
}

/**
 * @brief WriterClient::sendWriteRequest - catches signal from the ui button
 */
void WriterClient::sendWriteRequest()
{ pa->emit transactionRecord("sendWriteRequest()");
  if ( pa->pp == nullptr )
    { pa->emit transactionRecord("protocol not defined.");
      return;
    }
  // Compose bao based on protocol definition and ui contents and send it.
  // Unneeded components will be ignored by pa->compose
  BlockObjectMap inputs;
  BlockValueByteArray  userId( ui->wcId  ->text()       .toUtf8() ); inputs.insert( RCD_userId_b    , &userId     );
  BlockValueString recordText( ui->wcData->toPlainText().toUtf8() ); inputs.insert( RCD_recordText_s, &recordText );
  // more to come
  BaoSerial bao = pa->compose( RCD_writeRequest_o, inputs );
  if ( bao.size() > 0 )
    emit sendRequest( bao );
   else
    qWarning( "problem when composing writeRequest" );
}

/**
 * @brief WriterClient::receiveResponse
 * @param resp - from server
 */
void WriterClient::receiveResponse( QByteArray resp )
{ pa->emit transactionRecord("receiveResponse()");
  (void)resp;
  // TODO: show response/results on ui
}
