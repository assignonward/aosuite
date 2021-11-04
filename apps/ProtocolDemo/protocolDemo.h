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
#ifndef PROTOCOLDEMO_H
#define PROTOCOLDEMO_H

#include <QScrollArea>
#include "ui_protocolDemo.h"
#include "protocolParser.h"
#include "writerClient.h"
#include "writerServer.h"
#include "readerClient.h"
#include "readerServer.h"

namespace Ui {
class ProtocolDemo;
}

class ProtocolDemo : public QScrollArea
{ Q_OBJECT
public:
              ProtocolDemo( QWidget *cw = nullptr );
             ~ProtocolDemo();
        void  initReadFile();

signals:
        void  setProtocol( BaoSerial );

public slots:
        void  on_set_clicked();
        void  updateUi( ProtocolActor * ) {} // based on the collection of sendable/receivable items, make corresponding ui elements visible / invisible

public:
  QPointer<WriterClient> wc;
  QPointer<WriterServer> ws;
  QPointer<ReaderClient> rc;
  QPointer<ReaderServer> rs;
       Ui::ProtocolDemo *ui;
};

#endif // PROTOCOLDEMO_H
