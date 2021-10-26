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

class WriterClient : public ProtocolActor
{
    Q_OBJECT
public:
    explicit WriterClient(QObject *parent = nullptr) : ProtocolActor( RCD_actorWriterClient_o, parent ) {}

signals:
        void sendRequest( BaoSerial ); // sends request to the server

public slots:
        void sendWriteRequest();       // catches signal from the ui button
        void receiveResponse( BaoSerial );
};

class WriterServer : public ProtocolActor
{
    Q_OBJECT
public:
    explicit WriterServer(QObject *parent = nullptr) : ProtocolActor( RCD_actorWriterServer_o, parent ) {}

signals:
        void sendResponse( BaoSerial );

public slots:
        void receiveRequest( BaoSerial );
};

class ReaderClient : public ProtocolActor
{
    Q_OBJECT
public:
    explicit ReaderClient(QObject *parent = nullptr) : ProtocolActor( RCD_actorReaderClient_o, parent ) {}

signals:
        void sendRequest( BaoSerial ); // sends request to the server

public slots:
        void sendReadRequest();        // catches signal from the ui button
        void receiveResponse( BaoSerial );
};

class ReaderServer : public ProtocolActor
{
    Q_OBJECT
public:
    explicit ReaderServer(QObject *parent = nullptr) : ProtocolActor( RCD_actorReaderServer_o, parent ) {}

signals:
        void sendResponse( BaoSerial );

public slots:
        void receiveRequest( BaoSerial );
};

namespace Ui {
class ProtocolDemo;
}

class ProtocolDemo : public QScrollArea
{
    Q_OBJECT
public:
              ProtocolDemo( QWidget *cw = nullptr );
             ~ProtocolDemo();
        void  initReadFile();

signals:
        void  setProtocol( BaoSerial );

public slots:
        void  on_set_clicked();

public:
    WriterClient  wc;
    WriterServer  ws;
    ReaderClient  rc;
    ReaderServer  rs;
Ui::ProtocolDemo *ui;

};

#endif // PROTOCOLDEMO_H
